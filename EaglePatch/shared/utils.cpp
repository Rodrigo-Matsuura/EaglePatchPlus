#include "utils.h"
#include "logger.h"
#include <xinput.h>

void InitPrecisionTimer()
{
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		HMODULE hWinmm = LoadLibraryA("winmm.dll");
		if (hWinmm)
		{
			typedef UINT(WINAPI * pfnTimeBeginPeriod)(UINT uPeriod);
			auto pTimeBeginPeriod = (pfnTimeBeginPeriod)GetProcAddress(hWinmm, "timeBeginPeriod");
			if (pTimeBeginPeriod)
			{
				pTimeBeginPeriod(1);
				LogInfo("Precision timer initialized with timeBeginPeriod(1).");
			}
		}
	}
}

void LimitFramerate(int targetFps)
{
	if (targetFps <= 0)
		return;

	InitPrecisionTimer();

	static LARGE_INTEGER frequency = {};
	static LARGE_INTEGER targetTime = {};

	if (frequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&targetTime);
		return;
	}

	LONGLONG targetTicksPerFrame = (LONGLONG)((double)frequency.QuadPart / (double)targetFps);
	targetTime.QuadPart += targetTicksPerFrame;

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// If we fell significantly behind (e.g. loading screen, window moved, lag spike > 2 frames),
	// clamp target time to current time to avoid running unthrottled to catch up.
	if (currentTime.QuadPart > targetTime.QuadPart + (targetTicksPerFrame * 2))
	{
		targetTime = currentTime;
		return;
	}

	while (currentTime.QuadPart < targetTime.QuadPart)
	{
		double remaining = (double)(targetTime.QuadPart - currentTime.QuadPart) / (double)frequency.QuadPart;
		if (remaining > 0.0015)
		{
			Sleep(1);
		}
		else
		{
			YieldProcessor();
		}
		QueryPerformanceCounter(&currentTime);
	}
}

void ApplyCpuCoreLimit(int maxCores)
{
	DWORD_PTR processAffinityMask, systemAffinityMask;
	if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
	{
		// Default to up to 8 logical cores/threads if enabled with 1 or <= 0,
		// or custom requested count if > 1.
		// 8 cores prevents the >16 threads Scimitar crash while ensuring plenty of headroom
		// for game render, physics, audio and worker threads to prevent stutter/starvation.
		size_t targetCores = (maxCores > 1) ? (size_t)maxCores : 8;

		DWORD_PTR newMask = 0;
		size_t coresSelected = 0;
		for (size_t i = 0; i < sizeof(DWORD_PTR) * 8 && coresSelected < targetCores; i++)
		{
			if (systemAffinityMask & ((DWORD_PTR)1 << i))
			{
				newMask |= ((DWORD_PTR)1 << i);
				coresSelected++;
			}
		}
		if (newMask != 0)
		{
			SetProcessAffinityMask(GetCurrentProcess(), newMask);
			LogInfo("Applied CPU affinity limit to first %zu active cores (Mask: 0x%IX, requested: %d).",
				coresSelected, newMask, maxCores);
		}
	}
	else
	{
		LogWarn("Failed to get process affinity mask.");
	}
}

void NotifyUnsupportedVersion(const char* title, const char* message)
{
	typedef int (WINAPI *pfnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
	HMODULE hUser32 = GetModuleHandleA("user32.dll");
	if (!hUser32)
	{
		hUser32 = LoadLibraryA("user32.dll");
	}
	if (hUser32)
	{
		pfnMessageBoxA pMessageBoxA = (pfnMessageBoxA)GetProcAddress(hUser32, "MessageBoxA");
		if (pMessageBoxA)
		{
			pMessageBoxA(NULL, message, title, MB_OK | MB_ICONWARNING);
		}
	}
}
