#include "DroneControllerDI.h"
#include "CoreMinimal.h"
#include <atlstr.h>  

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

FDroneControllerDI* FDroneControllerDI::self = nullptr;

FDroneControllerDI::FDroneControllerDI()
	: nDevices(0)
	, dwUserIndex(0)
	, pState(new XINPUT_STATE())
	, m_directInput(nullptr)
{
	if (FDroneControllerDI::self == nullptr)
	{
		FDroneControllerDI::self = this;
	}
}

FDroneControllerDI::~FDroneControllerDI()
{
	SAFE_DELETE(pState);
	SAFE_RELEASE(m_directInput);
}

int FDroneControllerDI::Run(const FControllerData& ControllerData)
{
	UE4x360ce::Run();

	result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return 1;
	}

	UpdateDevices();

	SetDefaultControllerData(ControllerData);

	return 1;
}

int FDroneControllerDI::UpdateDevices()
{
	/** @brief	Update number of controllers */
	UINT nDevicesLocal = 0;
	// Update devices only on start or if plug un-plug during the game
	GetRawInputDeviceList(NULL, &nDevicesLocal, sizeof(RAWINPUTDEVICELIST));
	if (nDevicesLocal != nDevices)
	{
		// Reset number of devices
		nDevices = nDevicesLocal;

		// Reset Controlers Data Array
		ControllersData.clear();

		// Look for a force feedback device we can use 
		result = m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, FDroneControllerDI::EnumFFDevicesCallback, (void*)1, DIEDFL_ALLDEVICES);
		if (FAILED(result))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to EnumFFDevicesCallback"));
		}
	}

	return 1;
}

XINPUT_STATE* FDroneControllerDI::GetCurrentControllerState(bool bUseFirstIfNotFound)
{
	if (DefaultControllerData.GUID.empty())
	{
		if (bUseFirstIfNotFound)
		{
			UE4x360ce::XInputGetState(0, pState);
			return pState;
		}
		else
		{
			return nullptr;
		}
	}

	// FindController ID by
	int ID = DefaultControllerData.ID;
	if (ID < 0)
	{
		if (bUseFirstIfNotFound)
		{
			UE4x360ce::XInputGetState(0, pState);
			return pState;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		UE4x360ce::XInputGetState(ID, pState);
		
		if (pState == nullptr)
		{
			UE4x360ce::XInputGetState(0, pState);
			return pState;
		}
		else
		{
			return pState;
		}
	}
}

int FDroneControllerDI::GetControllerIDByGUID(const GUID* m_productid)
{
	return UE4x360ce::GetControllerIDByGUID(m_productid);
}


BOOL CALLBACK FDroneControllerDI::EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext)
{

	wchar_t szGUID[64] = { 0 };
	StringFromGUID2(pInst->guidProduct, szGUID, 64);

	// Convert to string
	wstring ws(szGUID);
	string guidProductStr(ws.begin(), ws.end());

	wstring wsProductName(pInst->tszProductName);
	string ProductName(wsProductName.begin(), wsProductName.end());
	int ControllerID = FDroneControllerDI::self->GetControllerIDByGUID(&pInst->guidProduct);

	// check config

	//FDroneControllerDI::self->ControllersData.push_back({ guidProductStr , ProductName });

	return DIENUM_CONTINUE;
}