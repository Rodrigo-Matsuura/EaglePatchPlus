#pragma once

#include <windows.h>
#include <xinput.h>
#include "logger.h"

void LimitFramerate(int targetFps);

template<typename TPad> inline void CheckXInputReconnect(TPad* pad)
{
	if (!pad || pad->m_PadState.Connected)
		return;

	static DWORD lastCheckTime = 0;
	DWORD now = GetTickCount();
	if (now - lastCheckTime < 1000)
		return;
	lastCheckTime = now;

	typedef DWORD(WINAPI * pfnXInputGetState)(DWORD dwUserIndex, XINPUT_STATE * pState);
	static pfnXInputGetState pGetState = nullptr;
	static bool attempted = false;

	if (!attempted)
	{
		attempted = true;
		HMODULE hXInput = LoadLibraryA("xinput1_3.dll");
		if (!hXInput)
			hXInput = LoadLibraryA("xinput1_4.dll");
		if (!hXInput)
			hXInput = LoadLibraryA("xinput9_1_0.dll");

		if (hXInput)
		{
			pGetState = (pfnXInputGetState)GetProcAddress(hXInput, "XInputGetState");
			LogInfo("XInput module loaded successfully.");
		}
		else
		{
			LogWarn("Failed to locate or load any XInput DLL.");
		}
	}

	if (!pGetState)
		return;

	XINPUT_STATE state;
	for (DWORD i = 0; i < 4; i++)
	{
		if (pGetState(i, &state) == ERROR_SUCCESS)
		{
			pad->m_PadIndex = i;
			pad->m_PadState.Connected = true;
			pad->m_PadState.Inserted = true;
			pad->m_PadState.Removed = false;
			LogInfo("XInput controller detected and connected on slot %lu.", i);
			return;
		}
	}
}

void ApplyCpuCoreLimit(int maxCores = 0);
void NotifyUnsupportedVersion(const char* title, const char* message);
