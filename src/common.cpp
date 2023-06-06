#include "bqqenv/common.hpp"

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

std::unique_ptr<wchar_t[]> GetQQPath() {
  unsigned long maxPath = MAX_PATH;
  std::unique_ptr<wchar_t[]> qqPath = std::make_unique<wchar_t[]>(MAX_PATH);

  long openKeyErr = RegGetValueW(
      HKEY_LOCAL_MACHINE,
      L"Microsoft\\Windows"
      L"\\CurrentVersion\\Uninstall\\QQ",
      L"DisplayIcon",
      RRF_RT_REG_SZ,
      nullptr,
      qqPath.get(),
      &maxPath);

  if (openKeyErr) {
    qqPath[0] = 0;
    openKeyErr = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows"
        L"\\CurrentVersion\\Uninstall\\QQ",
        L"DisplayIcon",
        RRF_RT_REG_SZ,
        nullptr,
        qqPath.get(),
        &maxPath);
  }

  if (openKeyErr) {
    SetLastError(openKeyErr);
    LogAndFailWithLastError(L"获取 QQ 位置时出现错误。QQ 可能未安装？");
  }

  // 去除 DisplayIcon 后的 ",0"
  qqPath[wcslen(qqPath.get()) - 2] = 0;

  return qqPath;
}
