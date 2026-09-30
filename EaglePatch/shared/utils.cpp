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
	static LARGE_INTEGER lastTime = {};

	if (frequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
		return;
	}

	double targetFrameTime = 1.0 / (double)targetFps;
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	double elapsedTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / (double)frequency.QuadPart;

	while (elapsedTime < targetFrameTime)
	{
		double remaining = targetFrameTime - elapsedTime;
		if (remaining > 0.0015)
		{
			Sleep(1);
		}
		else
		{
			YieldProcessor();
		}
		QueryPerformanceCounter(&currentTime);
		elapsedTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / (double)frequency.QuadPart;
	}

	lastTime = currentTime;
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
