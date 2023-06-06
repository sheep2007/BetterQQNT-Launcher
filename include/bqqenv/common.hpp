#ifndef _BQQENV_COMMON_
#define _BQQENV_COMMON_

#include "bqqenv/stdafx.hpp"

void LogAndFail(const wchar_t *message);
void LogAndFailWithLastError(const wchar_t *message);

std::unique_ptr<wchar_t[]> GetQQPath();

#endif /* _BQQENV_COMMON_ */
