#pragma once

// WinApi
#include <windows.h>

// DirectX
#pragma warning(push) 
#pragma warning(disable:6000 28251) 
#include <dinput.h> 
#pragma warning(pop)

#include <xinput.h>

#include <string>

using namespace std;

class UE4x360ce
{
public:
	static __declspec(dllexport) void Run();
	static __declspec(dllexport) DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
	static __declspec(dllexport) void XInputEnable(bool enable);
	static __declspec(dllexport) int GetControllerUserIndexByGUIDInstance(const GUID* guidInstance);
	static __declspec(dllexport) void Reset();
};