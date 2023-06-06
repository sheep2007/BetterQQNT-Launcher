#include "bqqenv/main.hpp"

int __stdcall wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow) {
  unsigned long maxPath = MAX_PATH;

  // 获取 QQ.exe 位置
  wchar_t qqPath[MAX_PATH];
  const long openKeyErr = RegGetValueW(
      HKEY_LOCAL_MACHINE,
      L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows"
      L"\\CurrentVersion\\Uninstall\\QQ",
      L"DisplayIcon",
      RRF_RT_REG_SZ,
      nullptr,
      &qqPath,
      &maxPath);
  if (openKeyErr) {
    SetLastError(openKeyErr);
    LogAndFailWithLastError(L"获取 QQ 位置时出现错误。QQ 可能未安装？");
  }

  // 去除 DisplayIcon 后的 ",0"
  qqPath[wcslen(qqPath) - 2] = 0;

  PROCESSENTRY32W pe;
  pe.dwSize = sizeof(PROCESSENTRY32W);
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  Process32FirstW(hSnap, &pe);
  bool peFound = false;
  bool findProcess = false;
  while (!findProcess) {
    findProcess = Process32NextW(hSnap, &pe);
    if (wcscmp(pe.szExeFile, qqPath) == 0) {
      peFound = true;
      findProcess = false;
    }
  }
  if (peFound) {
    // QQ 正在运行。退出
    return 0;
  }
  if (!CloseHandle(hSnap)) LogAndFailWithLastError(L"关闭 hSnap 失败。");

  // 加载 BetterQQNT dll
  HRSRC bqqntRc =
      FindResourceW(hInstance, MAKEINTRESOURCEW(102), MAKEINTRESOURCEW(256));
  HANDLE bqqntRcData = LoadResource(hInstance, bqqntRc);
  const unsigned long bqqntSize = SizeofResource(hInstance, bqqntRc);
  const char *bqqntData = static_cast<const char *>(LockResource(bqqntRcData));

  // 复制 BetterQQNT dll 到临时文件
  wchar_t tempPath[MAX_PATH];
  if (!GetTempPathW(MAX_PATH, tempPath))
    LogAndFailWithLastError(L"获取临时目录失败。");

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  const __int64 now = static_cast<__int64>(ft.dwLowDateTime) +
                      (static_cast<__int64>(ft.dwHighDateTime) << 32LL);
  const std::wstring bqqntPathStr =
      std::format(L"{}better-qqnt-{}.dll", tempPath, now);
  const wchar_t *bqqntPath = bqqntPathStr.c_str();
  const size_t bqqntPathSize =
      sizeof(wchar_t) * (bqqntPathStr.size() + 1);

  HANDLE bqqntFile =
      CreateFileW(bqqntPath, GENERIC_WRITE, 0, nullptr, CREATE_NEW, 0, 0);
  if (bqqntFile == INVALID_HANDLE_VALUE)
    LogAndFailWithLastError(L"创建临时文件失败。");
  unsigned long sizeWritten;
  if (!WriteFile(bqqntFile, bqqntData, bqqntSize, &sizeWritten, nullptr))
    LogAndFailWithLastError(L"写入临时文件失败。");
  if (!CloseHandle(bqqntFile)) LogAndFailWithLastError(L"关闭临时文件失败。");

  // 准备 QQ 启动命令行
  std::wstring cmdLineStr = std::format(L"\"{}\"", qqPath);
  // 检测是否为 WINE
  if (GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "wine_get_version")) {
    // 环境为 WINE，禁用 GPU 渲染
    cmdLineStr += L" --disable-gpu";
  }
  wchar_t cmdLine[32767];
  cmdLine[cmdLineStr.copy(cmdLine, 32766)] = 0;

  // 创建 QQ 进程
  STARTUPINFOW si;
  ZeroMemory(&si, sizeof(STARTUPINFOW));
  si.cb = sizeof(STARTUPINFOW);
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  if (!CreateProcessW(
          nullptr, cmdLine, nullptr, nullptr, 0, 0, nullptr, nullptr, &si, &pi))
    LogAndFailWithLastError(L"创建 QQ 进程失败。");

  // 向远程内存写入 BetterQQNT dll 地址
  void *remoteBqqntPath = VirtualAllocEx(
      pi.hProcess,
      nullptr,
      bqqntPathSize,
      MEM_RESERVE | MEM_COMMIT,
      PAGE_EXECUTE_READWRITE);
  if (!remoteBqqntPath) LogAndFailWithLastError(L"申请远程内存失败。");

  if (!WriteProcessMemory(
          pi.hProcess, remoteBqqntPath, bqqntPath, bqqntPathSize, nullptr))
    LogAndFailWithLastError(L"写入内存失败。");

  HANDLE remoteThread = CreateRemoteThread(
      pi.hProcess,
      nullptr,
      0,
      reinterpret_cast<LPTHREAD_START_ROUTINE>(
          GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW")),
      remoteBqqntPath,
      0,
      nullptr);
  if (!remoteThread) LogAndFailWithLastError(L"创建远程线程失败。");

  WaitForSingleObject(remoteThread, INFINITE);
  CloseHandle(remoteThread);

  if (!VirtualFreeEx(pi.hProcess, remoteBqqntPath, 0, MEM_RELEASE))
    LogAndFailWithLastError(L"释放远程内存失败。");
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  DeleteFileW(bqqntPath);

  return 0;
}

void LogAndFail(const wchar_t *message) {
  MessageBoxW(nullptr, message, L"BetterQQNT-Launcher 错误", MB_ICONERROR);
  ExitProcess(1);
}

void LogAndFailWithLastError(const wchar_t *message) {
  wchar_t *winErrMessage;
  const unsigned long err = GetLastError();
  FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      err,
      0,
      reinterpret_cast<wchar_t *>(&winErrMessage),
      0,
      nullptr);
  LogAndFail(
      std::format(L"{}错误代码：{} 详细信息：{}", message, err, winErrMessage)
          .c_str());
}
