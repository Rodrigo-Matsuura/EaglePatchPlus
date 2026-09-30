#pragma once

#include <windows.h>

enum class LogLevel
{
	Info,
	Warn,
	Error
};

void InitLogging(HMODULE hModule, bool enableFileLog, bool enableConsoleLog);
void LogMessage(LogLevel level, const char* format, ...);
void LogMessageV(LogLevel level, const char* format, va_list args);
void LogInfo(const char* format, ...);
void LogWarn(const char* format, ...);
void LogError(const char* format, ...);
void ShutdownLogging();
bool IsLoggingEnabled();
const wchar_t* GetLogFilePath();
