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
		string guidProduct;
		string ProductName;
	};

	FDroneControllerDI();
	~FDroneControllerDI();

private:
	static BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext);
public:
	int Run(const FControllerData& ControllerData);
	int UpdateDevices();
	XINPUT_STATE* GetCurrentControllerState(bool bUseFirstIfNotFound);
	int GetControllerUserIndexByGUID(const GUID* m_productid);
	void SetDefaultControllerData(const FControllerData& ControllerData);
	const vector<FControllerData>& GetControllersData() { return ControllersData; }

public:
	static string GUIDtoString(GUID m_productid);
	static GUID StringToGUID(const string& str);


private:
	vector<FControllerData> ControllersData;
	UINT nDevices;

	HRESULT result;
	DWORD dwUserIndex;
	XINPUT_STATE* pState;
	IDirectInput8* m_directInput;

private:
	FControllerData DefaultControllerData;

private:
	static FDroneControllerDI* self;
};