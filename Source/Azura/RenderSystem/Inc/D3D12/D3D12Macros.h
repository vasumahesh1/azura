#pragma once
#include <winerror.h>
#include <stdexcept>
#include "Types.h"


namespace Azura {
namespace D3D12 {

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline std::string HrToString(HRESULT hr)
{
  char s_str[64] = {};
  sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
  return std::string(s_str);
}

#define VERIFY_D3D_OP(Logger, expr, message)                                                           \
  {                                                                                                    \
    HRESULT hr =  expr;                                                                                \
    if (FAILED(hr))                                                                                    \
    {                                                                                                  \
      LOG_ERR(Logger, LOG_LEVEL, message);                                                             \
      LOG_ERR(Logger, LOG_LEVEL, HrToString(hr).c_str());                                              \
      throw std::runtime_error(message);                                                               \
    }                                                                                                  \
  }

} // namespace D3D12
} // namespace Azura