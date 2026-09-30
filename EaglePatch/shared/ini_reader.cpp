#include "ini_reader.h"
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef DLL_NAME
#define DLL_NAME "EaglePatch+"
#endif

static wchar_t ini_path[MAX_PATH];

static void TrimString(char* str)
{
	if (!str || !*str)
		return;

	// Trim leading whitespace
	char* start = str;
	while (*start && isspace((unsigned char)*start))
	{
		start++;
	}

	if (start != str)
	{
		memmove(str, start, strlen(start) + 1);
	}

	// Trim trailing whitespace
	size_t len = strlen(str);
	while (len > 0 && isspace((unsigned char)str[len - 1]))
	{
		str[--len] = '\0';
	}
}

static const wchar_t* AnsiToWideHelper(const char* ansi, wchar_t* wideBuffer, size_t wideSize)
{
	if (!ansi || !wideBuffer || wideSize == 0)
		return nullptr;
	if (MultiByteToWideChar(CP_ACP, 0, ansi, -1, wideBuffer, (int)wideSize) == 0)
	{
		wideBuffer[wideSize - 1] = L'\0';
	}
	return wideBuffer;
}

static void WideToAnsi(const wchar_t* wide, char* ansi, size_t ansiSize)
{
	if (!wide || !ansi || ansiSize == 0)
		return;
	if (WideCharToMultiByte(CP_ACP, 0, wide, -1, ansi, (int)ansiSize, NULL, NULL) == 0)
	{
		ansi[ansiSize - 1] = '\0';
	}
}

static void EnsureIniPathInitialized()
{
	if (ini_path[0] == L'\0')
	{
		init_private_profile(NULL);
	}
}

UINT get_private_profile_int(LPCTSTR lpKeyName, INT nDefault)
{
	EnsureIniPathInitialized();
	wchar_t wKeyName[128];
	wchar_t wSection[128];
	const wchar_t* pwKeyName = AnsiToWideHelper(lpKeyName, wKeyName, 128);
	const wchar_t* pwSection = AnsiToWideHelper(DLL_NAME, wSection, 128);
	return GetPrivateProfileIntW(pwSection, pwKeyName, nDefault, ini_path);
}

UINT get_private_profile_bool(LPCTSTR lpKeyName, INT nDefault)
{
	char value[64];
	get_private_profile_string(lpKeyName, nDefault ? "1" : "0", value, sizeof(value));
	TrimString(value);

	if (_stricmp(value, "true") == 0 || _stricmp(value, "yes") == 0 || _stricmp(value, "on") == 0 ||
		_stricmp(value, "enable") == 0 || _stricmp(value, "enabled") == 0 || strcmp(value, "1") == 0)
	{
		return TRUE;
	}
	if (_stricmp(value, "false") == 0 || _stricmp(value, "no") == 0 || _stricmp(value, "off") == 0 ||
		_stricmp(value, "disable") == 0 || _stricmp(value, "disabled") == 0 || strcmp(value, "0") == 0)
	{
		return FALSE;
	}
	return nDefault ? TRUE : FALSE;
}

DWORD get_private_profile_string(LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize)
{
	if (!lpReturnedString || nSize == 0)
		return 0;

	EnsureIniPathInitialized();

	wchar_t wKeyName[128];
	wchar_t wSection[128];
	wchar_t wDefault[128];

	const wchar_t* pwKeyName = AnsiToWideHelper(lpKeyName, wKeyName, 128);
	const wchar_t* pwSection = AnsiToWideHelper(DLL_NAME, wSection, 128);
	const wchar_t* pwDefault = AnsiToWideHelper(lpDefault, wDefault, 128);

	wchar_t wStackBuffer[512];
	wchar_t* wReturnedString = wStackBuffer;
	if (nSize > 512)
	{
		wReturnedString = (wchar_t*)malloc(nSize * sizeof(wchar_t));
		if (!wReturnedString)
			return 0;
	}

	GetPrivateProfileStringW(pwSection, pwKeyName, pwDefault, wReturnedString, nSize, ini_path);
	WideToAnsi(wReturnedString, lpReturnedString, nSize);

	if (wReturnedString != wStackBuffer)
	{
		free(wReturnedString);
	}
	return (DWORD)strlen(lpReturnedString);
}

FLOAT get_private_profile_float(LPCTSTR lpKeyName, LPCTSTR lpDefault)
{
	CHAR lpReturnedString[MAX_PATH];
	get_private_profile_string(lpKeyName, lpDefault, lpReturnedString, sizeof(lpReturnedString));
	TrimString(lpReturnedString);
	for (char* c = lpReturnedString; *c; c++)
	{
		if (*c == ',')
		{
			*c = '.';
		}
	}
	return (FLOAT)atof(lpReturnedString);
}

void init_private_profile(HMODULE hModule)
{
	GetModuleFileNameW(hModule, ini_path, sizeof(ini_path) / sizeof(wchar_t));
	wchar_t* p = wcsrchr(ini_path, L'.');
	if (p)
	{
		wcscpy_s(p, (sizeof(ini_path) / sizeof(wchar_t)) - (p - ini_path), L".ini");
	}
	else
	{
		wcsncat_s(ini_path, sizeof(ini_path) / sizeof(wchar_t), L".ini", 4);
	}
}

const wchar_t* get_ini_path()
{
	return ini_path;
}

void set_custom_ini_path(const wchar_t* path)
{
	if (path)
	{
		wcscpy_s(ini_path, sizeof(ini_path) / sizeof(wchar_t), path);
	}
}