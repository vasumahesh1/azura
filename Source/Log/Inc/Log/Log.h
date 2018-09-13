#pragma once
#include "Types.h"

namespace Azura {
enum class LogLevel {
  Debug,
  Info,
  Warning,
  Error,
  Fatal
};

class Log {
public:
  Log(String key);

  void Debug(U32 level, const String& str) const;
  void Info(U32 level, const String& str) const;
  void Warning(U32 level, const String& str) const;
  void Error(U32 level, const String& str) const;
  void Fatal(U32 level, const String& str) const;

private:
  void ParseLevelString(const String& levelStr);

  U8 m_debugLevel;
  U8 m_infoLevel;
  U8 m_warningLevel;
  U8 m_errorLevel;
  U8 m_fatalLevel;
  String m_key;
  bool m_isReady;
};

#ifdef BUILD_DEBUG

// TODO(vasumahesh1):[LOG]: Fix BOOST_LOG_NAMED_SCOPE("") here.
#define LOG_DEBUG(LOG, LEVEL, STR) \
  LOG.Debug(LEVEL, STR)

#define LOG_INFO(LOG, LEVEL, STR) \
  LOG.Info(LEVEL, STR)

#define LOG_WARNING(LOG, LEVEL, STR) \
  LOG.Warning(LEVEL, STR)

#define LOG_ERROR(LOG, LEVEL, STR) \
  LOG.Error(LEVEL, STR)

#define LOG_FATAL(LOG, LEVEL, STR) \
  LOG.Fatal(LEVEL, STR)

#define LOG_DBG(LOG, LEVEL, STR) LOG_DEBUG(LOG, LEVEL, STR)
#define LOG_INF(LOG, LEVEL, STR) LOG_INFO(LOG, LEVEL, STR)
#define LOG_WRN(LOG, LEVEL, STR) LOG_WARNING(LOG, LEVEL, STR)
#define LOG_ERR(LOG, LEVEL, STR) LOG_ERROR(LOG, LEVEL, STR)
#define LOG_FTL(LOG, LEVEL, STR) LOG_FATAL(LOG, LEVEL, STR)

#elif BUILD_RELEASE

#define LOG_DEBUG(LOG, LEVEL, STR) 

#define LOG_INFO(LOG, LEVEL, STR) 

#define LOG_WARNING(LOG, LEVEL, STR) 

#define LOG_ERROR(LOG, LEVEL, STR) \
  LOG.Error(LEVEL, STR)

#define LOG_FATAL(LOG, LEVEL, STR) \
  LOG.Fatal(LEVEL, STR)

#define LOG_DBG(LOG, LEVEL, STR) LOG_DEBUG(LOG, LEVEL, STR)
#define LOG_INF(LOG, LEVEL, STR) LOG_INFO(LOG, LEVEL, STR)
#define LOG_WRN(LOG, LEVEL, STR) LOG_WARNING(LOG, LEVEL, STR)
#define LOG_ERR(LOG, LEVEL, STR) LOG_ERROR(LOG, LEVEL, STR)
#define LOG_FTL(LOG, LEVEL, STR) LOG_FATAL(LOG, LEVEL, STR)

#endif

} // namespace Azura
