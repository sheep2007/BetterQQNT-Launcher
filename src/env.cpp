#include "bqqenv/common.hpp"

int __stdcall wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow) {
  const std::unique_ptr<wchar_t[]> qqPath = GetQQPath();

  unsigned long qqBinaryType;
  if (!GetBinaryTypeW(qqPath.get(), &qqBinaryType))
    LogAndFailWithLastError(L"检测 QQ 时出现错误。QQ 可能损坏？");

  const wchar_t *bqqenvID;
  switch (qqBinaryType) {
  case SCS_32BIT_BINARY:
    bqqenvID = MAKEINTRESOURCEW(102);
    break;
  case SCS_64BIT_BINARY:
    bqqenvID = MAKEINTRESOURCEW(103);
    break;
  default:
    LogAndFailWithLastError(L"检测 QQ 时出现错误。QQ 可能损坏？");
    break;
  }

  // 加载 launcher
  HRSRC bqqenvRc = FindResourceW(hInstance, bqqenvID, MAKEINTRESOURCEW(256));
  HANDLE bqqenvRcData = LoadResource(hInstance, bqqenvRc);
  const unsigned long bqqenvSize = SizeofResource(hInstance, bqqenvRc);
  const char *bqqenvData =
      static_cast<const char *>(LockResource(bqqenvRcData));

  // 复制 launcher 到临时文件
  wchar_t tempPath[MAX_PATH];
  if (!GetTempPathW(MAX_PATH, tempPath))
    LogAndFailWithLastError(L"获取临时目录失败。");

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  const __int64 now = static_cast<__int64>(ft.dwLowDateTime) +
                      (static_cast<__int64>(ft.dwHighDateTime) << 32LL);
  const std::wstring bqqntPathStr =
      std::format(L"{}bqqenv-{}.dll", tempPath, now);
  const wchar_t *bqqenvPath = bqqntPathStr.c_str();

  HANDLE bqqenvFile = CreateFileW(
      bqqenvPath, GENERIC_WRITE, 0, nullptr, CREATE_NEW, 0, nullptr);
  if (bqqenvFile == INVALID_HANDLE_VALUE)
    LogAndFailWithLastError(L"创建临时文件失败。");
  unsigned long sizeWritten;
  if (!WriteFile(bqqenvFile, bqqenvData, bqqenvSize, &sizeWritten, nullptr))
    LogAndFailWithLastError(L"写入临时文件失败。");
  if (!CloseHandle(bqqenvFile)) LogAndFailWithLastError(L"关闭临时文件失败。");

  // 准备 launcher 启动命令行
  std::wstring cmdLineStr = std::format(L"\"{}\"", bqqenvPath);
  wchar_t cmdLine[32767];
  cmdLine[cmdLineStr.copy(cmdLine, 32766)] = 0;

  // 创建 launcher 进程
  STARTUPINFOW si;
  ZeroMemory(&si, sizeof(STARTUPINFOW));
  si.cb = sizeof(STARTUPINFOW);
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  if (!CreateProcessW(
          nullptr, cmdLine, nullptr, nullptr, 0, 0, nullptr, nullptr, &si, &pi))
    LogAndFailWithLastError(L"创建 BetterQQNT-Launcher 进程失败。");

  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  DeleteFileW(bqqenvPath);

  return 0;
}
