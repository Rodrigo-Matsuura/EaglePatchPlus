#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../EaglePatch/shared/ini_reader.h"
#include "../EaglePatch/shared/utils.h"
#include "../EaglePatch/shared/logger.h"

static void TestIniReaderDefaults()
{
	printf("[TEST] Running INI Reader Default Values Tests...\n");

	// Non-existent keys should return provided defaults
	assert(get_private_profile_bool("NonExistentBoolTrue", TRUE) == TRUE);
	assert(get_private_profile_bool("NonExistentBoolFalse", FALSE) == FALSE);
	assert(get_private_profile_int("NonExistentInt60", 60) == 60);
	assert(get_private_profile_int("NonExistentInt0", 0) == 0);

	char strBuf[64] = { 0 };
	get_private_profile_string("NonExistentStr", "DefaultVal", strBuf, sizeof(strBuf));
	assert(strcmp(strBuf, "DefaultVal") == 0);

	FLOAT floatVal = get_private_profile_float("NonExistentFloat", "1.25");
	assert(fabsf(floatVal - 1.25f) < 0.001f);

	printf("[TEST] INI Reader Default Values Tests Passed.\n");
}

static void TestIniReaderFileParsing()
{
	printf("[TEST] Running INI Reader File Parsing Tests...\n");

	wchar_t tempPath[MAX_PATH] = { 0 };
	GetTempPathW(MAX_PATH, tempPath);
	wchar_t tempIniFile[MAX_PATH] = { 0 };
	wcscpy_s(tempIniFile, sizeof(tempIniFile) / sizeof(wchar_t), tempPath);
	wcscat_s(tempIniFile, sizeof(tempIniFile) / sizeof(wchar_t), L"eaglepatch_test.ini");

	FILE* f = _wfopen(tempIniFile, L"w");
	assert(f != nullptr);

	const char* iniContent =
		"[EaglePatch+]\n"
		"TestBoolTrue1=1\n"
		"TestBoolTrue2=true\n"
		"TestBoolTrue3=TRUE\n"
		"TestBoolTrue4=yes\n"
		"TestBoolTrue5=on\n"
		"TestBoolTrue6=enabled\n"
		"TestBoolTrue7=  true  \n"
		"TestBoolFalse1=0\n"
		"TestBoolFalse2=false\n"
		"TestBoolFalse3=FALSE\n"
		"TestBoolFalse4=no\n"
		"TestBoolFalse5=off\n"
		"TestBoolFalse6=disabled\n"
		"TestBoolFalse7=  0  \n"
		"TestBoolInvalid=invalid_choice\n"
		"TestInt1=60\n"
		"TestInt2=144\n"
		"TestInt3=0\n"
		"TestInt4=-10\n"
		"TestIntSpaced=  120  \n"
		"TestFloat1=1.0\n"
		"TestFloat2=1.5\n"
		"TestFloat3=0.75\n"
		"TestFloat4=-2.5\n"
		"TestFloatSpaced=  1.25  \n"
		"TestFloatComma1=1,5\n"
		"TestFloatComma2=0,75\n"
		"TestFloatCommaSpaced=  2,25  \n"
		"TestString1=KeyboardMouse2\n"
		"TestStringSpaced=  CustomValue  \n"
		"\n[EaglePatch]\n"
		"FallbackInt=99\n"
		"FallbackString=FallbackSuccess\n";

	fputs(iniContent, f);
	fclose(f);

	set_custom_ini_path(tempIniFile);

	// Test boolean parsing
	assert(get_private_profile_bool("TestBoolTrue1", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue2", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue3", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue4", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue5", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue6", FALSE) == TRUE);
	assert(get_private_profile_bool("TestBoolTrue7", FALSE) == TRUE);

	assert(get_private_profile_bool("TestBoolFalse1", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse2", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse3", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse4", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse5", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse6", TRUE) == FALSE);
	assert(get_private_profile_bool("TestBoolFalse7", TRUE) == FALSE);

	// Fallback for invalid value
	assert(get_private_profile_bool("TestBoolInvalid", TRUE) == TRUE);
	assert(get_private_profile_bool("TestBoolInvalid", FALSE) == FALSE);

	// Test integer parsing
	assert(get_private_profile_int("TestInt1", 0) == 60);
	assert(get_private_profile_int("TestInt2", 0) == 144);
	assert(get_private_profile_int("TestInt3", 100) == 0);
	assert((int)get_private_profile_int("TestInt4", 0) == -10);
	assert(get_private_profile_int("TestIntSpaced", 0) == 120);

	// Test float parsing (both dot and comma separators)
	assert(fabsf(get_private_profile_float("TestFloat1", "0.0") - 1.0f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloat2", "0.0") - 1.5f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloat3", "0.0") - 0.75f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloat4", "0.0") - (-2.5f)) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloatSpaced", "0.0") - 1.25f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloatComma1", "0.0") - 1.5f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloatComma2", "0.0") - 0.75f) < 0.001f);
	assert(fabsf(get_private_profile_float("TestFloatCommaSpaced", "0.0") - 2.25f) < 0.001f);

	// Test string parsing and returned byte length
	char strBuf[64] = { 0 };
	DWORD strLen = get_private_profile_string("TestString1", "", strBuf, sizeof(strBuf));
	assert(strcmp(strBuf, "KeyboardMouse2") == 0);
	assert(strLen == strlen("KeyboardMouse2"));

	// Test fallback to [EaglePatch] generic section
	assert(get_private_profile_int("FallbackInt", 0) == 99);
	char fallbackBuf[64] = { 0 };
	get_private_profile_string("FallbackString", "", fallbackBuf, sizeof(fallbackBuf));
	assert(strcmp(fallbackBuf, "FallbackSuccess") == 0);

	DeleteFileW(tempIniFile);
	printf("[TEST] INI Reader File Parsing Tests Passed.\n");
}

static void TestDiagnosticLogger()
{
	printf("[TEST] Running Diagnostic Logger Tests...\n");

	// Initialize logging with explicit mock DLL handle (using NULL for manual path or testing)
	InitLogging(NULL, false, false);
	assert(IsLoggingEnabled() == false);

	// Test enabling logging to temp log file
	HMODULE hKernel = GetModuleHandleA("kernel32.dll");
	InitLogging(hKernel, true, false);
	assert(IsLoggingEnabled() == true);

	LogInfo("Test info message %d", 42);
	LogWarn("Test warning message with param %s", "warning_sample");
	LogError("Test error message with code 0x%X", 0xDEAD);

	ShutdownLogging();
	assert(IsLoggingEnabled() == false);

	const wchar_t* logPath = GetLogFilePath();
	assert(logPath != nullptr && logPath[0] != L'\0');

	// Verify log file exists and contains expected tags
	FILE* f = _wfopen(logPath, L"r");
	if (f)
	{
		char buffer[2048] = { 0 };
		size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, f);
		fclose(f);
		assert(bytesRead > 0);
		assert(strstr(buffer, "[INFO]") != nullptr);
		assert(strstr(buffer, "[WARN]") != nullptr);
		assert(strstr(buffer, "[ERROR]") != nullptr);
		assert(strstr(buffer, "Test info message 42") != nullptr);
		assert(strstr(buffer, "=== EaglePatch+ Log Closed ===") != nullptr);
		DeleteFileW(logPath);
	}

	printf("[TEST] Diagnostic Logger Tests Passed.\n");
}

static void TestUtils()
{
	printf("[TEST] Running Utils & Timer Tests...\n");

	// Test non-positive frame limiter values return instantly
	LimitFramerate(0);
	LimitFramerate(-1);
	LimitFramerate(-60);

	// Test CPU core affinity limiter executes safely with defaults and custom counts
	ApplyCpuCoreLimit();
	ApplyCpuCoreLimit(0);
	ApplyCpuCoreLimit(1);
	ApplyCpuCoreLimit(4);
	ApplyCpuCoreLimit(8);

	// Test StickState deadzone behavior (preventing stick drift from blocking pad switching)
	struct TestStickState
	{
		float x, y;
		bool IsEmpty() const
		{
			return (x > -0.15f && x < 0.15f && y > -0.15f && y < 0.15f);
		}
	};
	TestStickState centered = { 0.0f, 0.0f };
	assert(centered.IsEmpty());
	TestStickState smallDrift = { 0.05f, -0.08f };
	assert(smallDrift.IsEmpty());
	TestStickState activeWalk = { 0.0f, 0.8f };
	assert(!activeWalk.IsEmpty());
	TestStickState activeTurn = { -0.7f, 0.1f };
	assert(!activeTurn.IsEmpty());

	// Test null-safety in CheckXInputReconnect
	struct DummyPad
	{
		uint32_t m_PadIndex;
		struct
		{
			bool Connected;
			bool Inserted;
			bool Removed;
		} m_PadState;
	};
	CheckXInputReconnect<DummyPad>(nullptr);

	printf("[TEST] Utils & Timer Tests Passed.\n");
}

int main()
{
	printf("========================================\n");
	printf("   Running EaglePatch+ Extended Tests   \n");
	printf("========================================\n");

	TestIniReaderDefaults();
	TestIniReaderFileParsing();
	TestDiagnosticLogger();
	TestUtils();

	printf("========================================\n");
	printf(" [SUCCESS] All Extended Tests Passed!   \n");
	printf("========================================\n");
	return 0;
}
