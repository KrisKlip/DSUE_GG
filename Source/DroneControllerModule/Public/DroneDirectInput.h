#pragma once

// Std
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <stdio.h> 
#include <vector>

// WinApi
#include <windows.h> 

// DirectX
#pragma warning(push) 
#pragma warning(disable:6000 28251) 
#include <dinput.h> 
#pragma warning(pop) 

using namespace std;

/**
 * @class	FDroneDirectInput
 *
 * @brief	DirectInput provider
 */

class FDroneDirectInput
{
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

	struct ControllerData {
		string GUID;
		string Name;
	};

public:
	FDroneDirectInput();
	~FDroneDirectInput();

	void Run();
	void Stop();

public:
	inline int GetDeviceNum() { return Controllers.size(); };

private:
	void InputThreadLoop();

private:
	thread TInputLoop;
	atomic_bool bIsInputLoopRunning;

private:
	vector<ControllerData> Controllers;
};