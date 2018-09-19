#pragma once

// Std
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <stdio.h> 
#include <vector>
#include <iostream>

// WinApi
#include <windows.h> 

// DirectX
#pragma warning(push) 
#pragma warning(disable:6000 28251) 
#include <dinput.h> 
#pragma warning(pop)

#include <xinput.h>

using namespace std;

struct __declspec(dllexport) UE4x360ceFrame
{
	int ControllersCount = 0;
};


class UE4x360ce
{
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

public:
	struct ControllerData {
		string GUID;
		string Name;
	};

	__declspec(dllexport) UE4x360ce();
	__declspec(dllexport) ~UE4x360ce();

	__declspec(dllexport) DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);

public:
	__declspec(dllexport) int Run();
	__declspec(dllexport) int Stop();

private:
	static BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext);

public:
	UE4x360ceFrame ue4x360ceFrame;

private:
	int InputThreadLoop();

private:
	thread TInputLoop;
	atomic_bool bIsInputLoopRunning;
	mutex TMutex;

private:
	vector<ControllerData> Controllers;
	UINT nDevices;
	int MaxControlelrs;

	HRESULT result;
	DWORD dwUserIndex;
	XINPUT_STATE* pState;
	IDirectInput8* m_directInput;

private:
	static UE4x360ce* self;
};