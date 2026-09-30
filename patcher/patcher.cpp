#include <windows.h>

#include "patcher.h"

StaticPatcher* StaticPatcher::ms_head;

StaticPatcher::StaticPatcher(Patcher func)
	: m_func(func)
{
	m_next = ms_head;
	ms_head = this;
}

void StaticPatcher::Apply()
{
	StaticPatcher* current = ms_head;

	while (current)
	{
		current->Run();
		current = current->m_next;
	}

	ms_head = NULL;
}

BOOL Unprotect_internal(void* address, size_t size, DWORD* oldProtect)
{
	return VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, oldProtect);
}

BOOL Protect_internal(void* address, size_t size, DWORD oldProtect)
{
	DWORD dummy;
	BOOL result = VirtualProtect(address, size, oldProtect, &dummy);
	FlushInstructionCache(GetCurrentProcess(), address, size);
	return result;
}
