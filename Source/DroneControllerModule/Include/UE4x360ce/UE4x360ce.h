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
	static __declspec(dllexport) bool GUIDtoString_helper(std::string* out, const GUID &g);
};