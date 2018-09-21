#include "DroneControllerDI.h"
#include "CoreMinimal.h"
#include <atlstr.h>  

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

FDroneControllerDI* FDroneControllerDI::self = nullptr;

FDroneControllerDI::FDroneControllerDI()
	: nDevices(0)
	, ControllersCount(0)
	, MaxControlelrs(4)
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

int FDroneControllerDI::Run()
{
	UE4x360ce::Run();

	result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return 1;
	}

	return 1;
}

int FDroneControllerDI::Update()
{
	/** @brief	Update number of controllers */
	UINT nDevicesLocal = 0;
	// Update devices only on start or if plug un-plug during the game
	GetRawInputDeviceList(NULL, &nDevicesLocal, sizeof(RAWINPUTDEVICELIST));
	if (nDevicesLocal != nDevices)
	{
		// Reset number of devices
		nDevices = nDevicesLocal;
		ControllersCount = 0;

		// Look for a force feedback device we can use 
		result = m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, FDroneControllerDI::EnumFFDevicesCallback, (void*)1, DIEDFL_ALLDEVICES);
		if (FAILED(result))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to EnumFFDevicesCallback"));
		}
	}

	// recieve input from all controlelrs
	for (int i = 0; i < ControllersCount; ++i)
	{
		UE4x360ce::XInputGetState(i, pState);
		if (pState != nullptr)
		{
			if (pState->Gamepad.wButtons)
			{
				UE_LOG(LogTemp, Warning, TEXT("GamePad %d"), pState->Gamepad.wButtons);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No pState"));
		}
	}

	return 1;
}


BOOL CALLBACK FDroneControllerDI::EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext)
{

	wchar_t szGUID[64] = { 0 };
	StringFromGUID2(pInst->guidProduct, szGUID, 64);

	// Convert to string
	wstring ws(szGUID);
	string guidProductStr(ws.begin(), ws.end());

	if (FDroneControllerDI::self->ControllersCount < FDroneControllerDI::self->MaxControlelrs)
	{
		FDroneControllerDI::self->ControllersCount++;
	}

	return DIENUM_CONTINUE;
}