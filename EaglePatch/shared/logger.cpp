#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

static bool g_LoggingInitialized = false;
static bool g_FileLoggingEnabled = false;
static bool g_ConsoleLoggingEnabled = false;
static wchar_t g_LogFilePath[MAX_PATH] = { 0 };
static CRITICAL_SECTION g_LogCriticalSection;
static bool g_CsInitialized = false;

static void FormatHelper(char* dst, size_t dstSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_vsnprintf(dst, dstSize - 1, format, args);
	va_end(args);
	dst[dstSize - 1] = '\0';
}

void InitLogging(HMODULE hModule, bool enableFileLog, bool enableConsoleLog)
{
	if (!g_CsInitialized)
	{
		InitializeCriticalSection(&g_LogCriticalSection);
		g_CsInitialized = true;
	}

	EnterCriticalSection(&g_LogCriticalSection);

	g_FileLoggingEnabled = enableFileLog;
	g_ConsoleLoggingEnabled = enableConsoleLog;

	GetModuleFileNameW(hModule, g_LogFilePath, sizeof(g_LogFilePath) / sizeof(wchar_t));
	wchar_t* p = wcsrchr(g_LogFilePath, L'.');
	if (p)
	{
		wcscpy_s(p, (sizeof(g_LogFilePath) / sizeof(wchar_t)) - (p - g_LogFilePath), L".log");
	}
	else
	{
		wcsncat_s(g_LogFilePath, sizeof(g_LogFilePath) / sizeof(wchar_t), L".log", 4);
	}

	g_LoggingInitialized = true;

	if (g_FileLoggingEnabled && g_LogFilePath[0] != L'\0')
	{
		FILE* f = _wfopen(g_LogFilePath, L"w");
		if (f)
		{
			time_t rawTime;
			time(&rawTime);
			struct tm timeInfo;
			char timeBuf[64] = { 0 };
			if (localtime_s(&timeInfo, &rawTime) == 0)
			{
				strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &timeInfo);
			}
			fprintf(f, "=== EaglePatch+ Diagnostic Log Started at %s ===\n", timeBuf[0] ? timeBuf : "Unknown time");
			fclose(f);
		}
	}

	LeaveCriticalSection(&g_LogCriticalSection);
}

void LogMessageV(LogLevel level, const char* format, va_list args)
{
	if (!g_LoggingInitialized || (!g_FileLoggingEnabled && !g_ConsoleLoggingEnabled))
		return;

	if (g_CsInitialized)
		EnterCriticalSection(&g_LogCriticalSection);

	char msgBuffer[1024];
	_vsnprintf(msgBuffer, sizeof(msgBuffer) - 1, format, args);
	msgBuffer[sizeof(msgBuffer) - 1] = '\0';

	const char* levelStr = "INFO";
	switch (level)
	{
		case LogLevel::Warn:
			levelStr = "WARN";
			break;
		case LogLevel::Error:
			levelStr = "ERROR";
			break;
		case LogLevel::Info:
		default:
			levelStr = "INFO";
			break;
	}

	time_t rawTime;
	time(&rawTime);
	struct tm timeInfo;
	char timeBuf[64] = { 0 };
	if (localtime_s(&timeInfo, &rawTime) == 0)
	{
		strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &timeInfo);
	}

	char formattedLine[1200];
	FormatHelper(formattedLine, sizeof(formattedLine), "[%s] [%s] %s\n", timeBuf[0] ? timeBuf : "??", levelStr, msgBuffer);

	if (g_ConsoleLoggingEnabled)
	{
		printf("%s", formattedLine);
	}

	if (g_FileLoggingEnabled && g_LogFilePath[0] != L'\0')
	{
		FILE* f = _wfopen(g_LogFilePath, L"a");
		if (f)
		{
			fputs(formattedLine, f);
			fflush(f);
			fclose(f);
		}
	}

	if (g_CsInitialized)
		LeaveCriticalSection(&g_LogCriticalSection);
}

void LogMessage(LogLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessageV(level, format, args);
	va_end(args);
}

void LogInfo(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessageV(LogLevel::Info, format, args);
	va_end(args);
}

void LogWarn(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessageV(LogLevel::Warn, format, args);
	va_end(args);
}

void LogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessageV(LogLevel::Error, format, args);
	va_end(args);
}

void ShutdownLogging()
{
	if (g_CsInitialized)
	{
		EnterCriticalSection(&g_LogCriticalSection);
		if (g_FileLoggingEnabled && g_LogFilePath[0] != L'\0')
		{
			FILE* f = _wfopen(g_LogFilePath, L"a");
			if (f)
			{
				fputs("=== EaglePatch+ Log Closed ===\n", f);
				fclose(f);
			}
		}
		g_LoggingInitialized = false;
		g_FileLoggingEnabled = false;
		g_ConsoleLoggingEnabled = false;
		LeaveCriticalSection(&g_LogCriticalSection);
		DeleteCriticalSection(&g_LogCriticalSection);
		g_CsInitialized = false;
	}
}

bool IsLoggingEnabled()
{
	return g_LoggingInitialized && (g_FileLoggingEnabled || g_ConsoleLoggingEnabled);
}

const wchar_t* GetLogFilePath()
{
	return g_LogFilePath;
}
