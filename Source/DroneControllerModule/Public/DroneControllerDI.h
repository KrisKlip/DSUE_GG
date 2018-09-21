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
	struct ControllerData {
		string GUID;
		string Name;
	};

	FDroneControllerDI();
	~FDroneControllerDI();

private:
	static BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext);
public:
	int Run();
	int Update();

private:
	vector<ControllerData> Controllers;
	UINT nDevices;
	int MaxControlelrs;
	int ControllersCount;

	HRESULT result;
	DWORD dwUserIndex;
	XINPUT_STATE* pState;
	IDirectInput8* m_directInput;

private:
	static FDroneControllerDI* self;
};