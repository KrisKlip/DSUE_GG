#include "DroneControllerDI.h"
#include "CoreMinimal.h"
#include <atlstr.h>
#include <cstdint>

#include <dbt.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

FDroneControllerDI* FDroneControllerDI::self = nullptr;

FDroneControllerDI::FDroneControllerDI()
	: dwUserIndex(0)
	, pState(new XINPUT_STATE())
	, m_directInput(nullptr)
	, bNeedsControllerStateUpdate(true)
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

	// Destroy dummy evnts window
	if (ELWindow != nullptr)
	{
		DestroyWindow(ELWindow);
	}
	UnregisterClass(ELWindowClassName, hInstance);
}

int FDroneControllerDI::Run(const FControllerData& ControllerData)
{
	UE4x360ce::Run();

	// For testing disable all inputs from Xinput devices only via Direact input
	//UE4x360ce::XInputEnable(false);

	result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return 1;
	}

	// 1.
	CreateELWindow();

	// 2.
	SetDefaultController(ControllerData);

	// 3.
	UpdateDevices();

	return 1;
}

void FDroneControllerDI::SetDefaultController(const FControllerData& ControllerData)
{
	DefaultControllerData = ControllerData;


	// find UserIndex by GUID
	if (!DefaultControllerData.guidInstance.empty())
	{
		GUID guid = StringToGUID(DefaultControllerData.guidInstance);
		int ConfigUserIndex = FDroneControllerDI::self->GetControllerUserIndexByGUIDInstance(&guid);

		DefaultControllerData.ConfigUserIndex = ConfigUserIndex;
	}
}

void FDroneControllerDI::UpdateDefaultController(const FControllerData& ControllerData)
{
	// 1.
	SetDefaultController(ControllerData);
	// 2.
	UpdateDevices();
	// 3. Update in ini file
}

void FDroneControllerDI::UpdateDefaultInActiveControllers()
{
	// Update Default controllers array
	for (int i = 0; i < ActiveControllersData.size(); ++i)
	{
		// First controller is default all the time if no default controller is found in active controllers
		if (i == 0)
		{
			ActiveControllersData[i].bIsDefault = true;
		}
		else
		{
			if (ActiveControllersData[i].guidInstance.compare(DefaultControllerData.guidInstance) == 0)
			{
				// reset default controller in array
				ActiveControllersData[i].bIsDefault = true;
				ActiveControllersData[0].bIsDefault = false;

				break;
			}
		}
	}
}

int FDroneControllerDI::GetActiveDefaultControllerUserIndex()
{
	int r = -1;

	for (auto ActiveControllerData : ActiveControllersData)
	{
		if (ActiveControllerData.bIsDefault)
		{
			return ActiveControllerData.ConfigUserIndex;
		}
	}

	return r;
}

void FDroneControllerDI::CreateELWindow()
{
	//Make a dummy window, this is the only way I know to register for window events (successfully listening to WM_DEVICECHANGE messages)
	ELClass.style = CS_VREDRAW;
	ELClass.lpfnWndProc = &FDroneControllerDI::WinProcCallback;
	ELClass.cbClsExtra = 0;
	ELClass.cbWndExtra = 0;
	ELClass.hInstance = hInstance;
	ELClass.hIcon = NULL;
	ELClass.hCursor = NULL;
	ELClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	ELClass.lpszMenuName = NULL;
	ELClass.lpszClassName = ELWindowClassName;
	if (!RegisterClass(&ELClass)) return;

	ELWindow = CreateWindow(ELWindowClassName, NULL, WS_MINIMIZE, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (ELWindow == NULL) return;

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

	NotificationFilter.dbcc_size = sizeof(NotificationFilter);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_reserved = 0;

	NotificationFilter.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72,
		0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };

	HDEVNOTIFY hDevNotify = RegisterDeviceNotification(NULL, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);

	UE_LOG(LogTemp, Warning, TEXT("CreateELWindow."));
}

INT_PTR FDroneControllerDI::WinProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {

	case WM_DEVICECHANGE:
	{
		UE_LOG(LogTemp, Warning, TEXT("WM_DEVICECHANGE"));
		UE4x360ce::Reset(); // restart config and controllers
		FDroneControllerDI::self->UpdateDevices(); // Update active directinput devices
		FDroneControllerDI::self->bNeedsControllerStateUpdate = true;
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void FDroneControllerDI::UpdateDevices()
{
	/** @brief	Update number of controllers */
	// Reset Controlers Data Array
	ActiveControllersData.clear();

	// Look for a force feedback device we can use 
	result = m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, FDroneControllerDI::EnumFFDevicesCallback, (void*)1, DIEDFL_ALLDEVICES);
	if (FAILED(result))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to EnumFFDevicesCallback"));
	}

	// Update Controllers
	UpdateDefaultInActiveControllers();
}

XINPUT_STATE* FDroneControllerDI::GetCurrentControllerState(bool bUseFirstIfNotFound)
{
	if (DefaultControllerData.guidInstance.empty())
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
	int ConfigUserIndex = GetActiveDefaultControllerUserIndex();
	if (ConfigUserIndex < 0)
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
		// Check if defalut device in array of active cevices

		UE4x360ce::XInputGetState(ConfigUserIndex, pState);
		
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

int FDroneControllerDI::GetControllerUserIndexByGUIDInstance(const GUID* guidInstance)
{
	return UE4x360ce::GetControllerUserIndexByGUIDInstance(guidInstance);
}


BOOL CALLBACK FDroneControllerDI::EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext)
{
	// Convert GUID to string
	string guidInstanceStr = FDroneControllerDI::GUIDtoString(pInst->guidInstance);

	wstring wsProductName(pInst->tszProductName);
	string ProductName(wsProductName.begin(), wsProductName.end());
	int ConfigUserIndex = FDroneControllerDI::self->GetControllerUserIndexByGUIDInstance(&pInst->guidInstance);

	FDroneControllerDI::self->ActiveControllersData.push_back({ ConfigUserIndex, guidInstanceStr, ProductName, false });

	return DIENUM_CONTINUE;
}

string FDroneControllerDI::GUIDtoString(GUID m_productid)
{
	wchar_t szGUID[64] = { 0 };
	StringFromGUID2(m_productid, szGUID, 64);

	// Convert to string
	wstring ws(szGUID);
	ws.pop_back();
	ws.erase(ws.begin());

	return string(ws.begin(), ws.end());
}

GUID FDroneControllerDI::StringToGUID(const std::string& str)
{
	CString guidstr = CString(string("{"+ str + "}").c_str());
	GUID guid;
	HRESULT hr = CLSIDFromString(guidstr.GetBuffer(), (LPCLSID)&guid);
	if (hr != S_OK)
	{
		// GUID string was invalid
	}

	return guid;
}