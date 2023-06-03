#ifndef _BQQENV_MAIN_
#define _BQQENV_MAIN_

#include <format>

#include <Windows.h>

// Must include after Windows.h
#include <TlHelp32.h>

int __stdcall wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow);

void LogAndFail(const wchar_t *message);
void LogAndFailWithLastError(const wchar_t *message);

#endif /* _BQQENV_MAIN_ */
