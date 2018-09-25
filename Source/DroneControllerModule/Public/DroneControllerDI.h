#pragma once

#include "UE4x360ce.h"

// Std
#include <string>
#include <stdio.h> 
#include <vector>
#include <memory>

using namespace std;

class FDroneControllerDI
{
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

public:
	struct FControllerData {
		int ConfigUserIndex;
		string guidInstance;
		string ProductName;
		bool bIsDefault;
	};

	FDroneControllerDI();
	~FDroneControllerDI();

private:
	static BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext);
public:
	int Run(const FControllerData& ControllerData);
	void UpdateDevices();
	XINPUT_STATE* GetCurrentControllerState(bool bUseFirstIfNotFound);
	int GetControllerUserIndexByGUIDInstance(const GUID* guidInstance);
	void SetDefaultController(const FControllerData& ControllerData);
	void UpdateDefaultController(const FControllerData& ControllerData);
	const vector<FControllerData>& GetControllersData() { return ActiveControllersData; }
public:
	/** If we've been notified by the system that the controller state may have changed */
	bool bNeedsControllerStateUpdate;

public:
	static string GUIDtoString(GUID m_productid);
	static GUID StringToGUID(const string& str);

private:
	void UpdateDefaultInActiveControllers();
	int GetActiveDefaultControllerUserIndex();

private:
	vector<FControllerData> ActiveControllersData;

	HRESULT result;
	DWORD dwUserIndex;
	XINPUT_STATE* pState;
	IDirectInput8* m_directInput;

private:
	FControllerData DefaultControllerData;

private:
	static FDroneControllerDI* self;

// Windows event listener window
private:
	LPTSTR ELWindowClassName = TEXT("ELWindow");
	WNDCLASS ELClass;
	HWND ELWindow;
private:
	void CreateELWindow();
	static INT_PTR WINAPI WinProcCallback(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);
};