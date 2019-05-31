#pragma once
#include "Types.h"
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

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

  void Debug(U32 level, const char* fmt, ...) const;
  
  void Info(U32 level, const char* fmt, ...) const;

  void Warning(U32 level, const char* fmt, ...) const;
  
  void Error(U32 level, const char* fmt, ...) const;

  void Fatal(U32 level, const char* fmt, ...) const;

private:
  void ParseLevelString(const String& levelStr);

  U8 m_debugLevel;
  U8 m_infoLevel;
  U8 m_warningLevel;
  U8 m_errorLevel;
  U8 m_fatalLevel;
  String m_key;
  bool m_isReady;

  std::vector<char> m_temporaryBuffer;
};

#if defined BUILD_DEBUG || defined FORCE_LOGGING

// TODO(vasumahesh1):[LOG]: Fix BOOST_LOG_NAMED_SCOPE("") here.
#define LOG_DEBUG(LOG, LEVEL, STR, ...) \
  LOG.Debug(LEVEL, STR, ## __VA_ARGS__) // NOLINT

#define LOG_INFO(LOG, LEVEL, STR, ...) \
  LOG.Info(LEVEL, STR, ## __VA_ARGS__) // NOLINT

#define LOG_WARNING(LOG, LEVEL, STR, ...) \
  LOG.Warning(LEVEL, STR, ## __VA_ARGS__) // NOLINT

#define LOG_ERROR(LOG, LEVEL, STR, ...) \
  LOG.Error(LEVEL, STR, ## __VA_ARGS__) // NOLINT

#define LOG_FATAL(LOG, LEVEL, STR, ...) \
  LOG.Fatal(LEVEL, STR, ## __VA_ARGS__) // NOLINT

#define LOG_DBG(LOG, LEVEL, STR, ...) LOG_DEBUG(LOG, LEVEL, STR, ## __VA_ARGS__)
#define LOG_INF(LOG, LEVEL, STR, ...) LOG_INFO(LOG, LEVEL, STR, ## __VA_ARGS__)
#define LOG_WRN(LOG, LEVEL, STR, ...) LOG_WARNING(LOG, LEVEL, STR, ## __VA_ARGS__)
#define LOG_ERR(LOG, LEVEL, STR, ...) LOG_ERROR(LOG, LEVEL, STR, ## __VA_ARGS__)
#define LOG_FTL(LOG, LEVEL, STR, ...) LOG_FATAL(LOG, LEVEL, STR, ## __VA_ARGS__)

#elif BUILD_RELEASE

#define LOG_DEBUG(LOG, LEVEL, STR, ...) 

#define LOG_INFO(LOG, LEVEL, STR, ...) 

#define LOG_WARNING(LOG, LEVEL, STR, ...) 

#define LOG_ERROR(LOG, LEVEL, STR, ...) \
  LOG.Error(LEVEL, STR, __VA_ARGS__) // NOLINT

#define LOG_FATAL(LOG, LEVEL, STR, ...) \
  LOG.Fatal(LEVEL, STR, __VA_ARGS__) // NOLINT

#define LOG_DBG(LOG, LEVEL, STR, ...) LOG_DEBUG(LOG, LEVEL, STR, __VA_ARGS__)
#define LOG_INF(LOG, LEVEL, STR, ...) LOG_INFO(LOG, LEVEL, STR, __VA_ARGS__)
#define LOG_WRN(LOG, LEVEL, STR, ...) LOG_WARNING(LOG, LEVEL, STR, __VA_ARGS__)
#define LOG_ERR(LOG, LEVEL, STR, ...) LOG_ERROR(LOG, LEVEL, STR, __VA_ARGS__)
#define LOG_FTL(LOG, LEVEL, STR, ...) LOG_FATAL(LOG, LEVEL, STR, __VA_ARGS__)

#endif

} // namespace Azura

#pragma clang diagnostic pop
