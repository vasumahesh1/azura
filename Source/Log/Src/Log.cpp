#include "Log/Log.h"

#include <iostream>
#include <cstdlib>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include "yaml-cpp/yaml.h"

namespace fs = boost::filesystem;

namespace Azura {

Log::Log(String key)
  : m_debugLevel(0),
    m_infoLevel(0),
    m_warningLevel(0),
    m_errorLevel(0),
    m_fatalLevel(0),
    m_key(std::move(key)),
    m_isReady(false) {

  const char* configEnv = std::getenv("AZURA_CONFIG");
  if (configEnv == nullptr) {
    std::cout << "No Azura Config Specified. Log Module will run in Default Mode. \n";
    return;
  }

  const fs::path configDir(configEnv);
  if (!is_directory(configDir)) {
    std::cout << "Azura Config Directory not present. Log Module will run in Default Mode. \n";
    return;
  }

  const fs::path fullPath = configDir / fs::path("log.yml");
  if (!fs::exists(fullPath)) {
    return;
  }

  static const YAML::Node s_mainConfig = [&]() -> YAML::Node
  {
    YAML::Node mainConfig = YAML::LoadFile(fullPath.string());
    if (!mainConfig) {
      std::cout << "Invalid YAML in log.yml. \n";
      return {};
    }

    return mainConfig;
  }();

  if (s_mainConfig.IsNull()) {
    return;
  }

  boost::log::add_common_attributes();
  boost::log::core::get()->add_global_attribute("Scope", boost::log::attributes::named_scope());

  try {
    bool isActive = false;
    YAML::Node logNode;
    YAML::Node logStreamsNode = s_mainConfig["LogStreams"];
    YAML::Node settingsNode   = s_mainConfig["Settings"];
    const String outputDir   = settingsNode["OutputDir"].as<String>();
    const auto flushLogs   = settingsNode["AutoFlush"].as<bool>();

    const fs::path fileLogOutputPath = fs::path(outputDir);

    auto fmtTimeStamp = boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
    auto fmtThreadId = boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");
    auto fmtSeverity = boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity");
    
    auto fmtScope = boost::log::expressions::format_named_scope("Scope",
      boost::log::keywords::format = "%n(%f:%l)",
      boost::log::keywords::iteration = boost::log::expressions::reverse,
      boost::log::keywords::depth = 2);

    const boost::log::formatter logFmt =
      boost::log::expressions::format("[%1%] (%2%) [%3%] [%4%] %5%")
      % fmtTimeStamp % fmtThreadId % fmtSeverity % fmtScope
      % boost::log::expressions::smessage;

    // Format of Log Stream:
    //  Settings:
    //    AutoFlush: true
    //
    //  LogStreams:
    //    VkRenderer:
    //      Levels: D0 I30 W50 E0 F0
    //      Streams:
    //        - File
    //        - Console
    //
    //    Common:
    //      Levels: D0 I50 W50 E0 F0
    //      Streams:
    //        - File
    //        - Console
    for (YAML::const_iterator itr = logStreamsNode.begin(); itr != logStreamsNode.end(); ++itr) {
      const String activeLogKey   = itr->first.as<String>();

      if (m_key == activeLogKey) {
        logNode  = itr->second;
        isActive = true;
        break;
      }
    }

    if (!isActive) {
      return;
    }

    const auto levelString   = logNode["Levels"].as<String>();
    const auto logKeyStreams = logNode["Streams"];

    ParseLevelString(levelString);

    for (YAML::const_iterator itr = logKeyStreams.begin(); itr != logKeyStreams.end(); ++itr) {
      const String logKeyStream   = itr->as<String>();

      if (logKeyStream == "File") {
        const fs::path finalPath = fileLogOutputPath / fs::path(m_key + ".log");
        auto fsSink = boost::log::add_file_log(finalPath.string());
        fsSink->locked_backend()->auto_flush(flushLogs);
        fsSink->set_formatter(logFmt);
      } else if (logKeyStream == "Console") {
        auto consoleSink = boost::log::add_console_log(std::clog);
        consoleSink->set_formatter(logFmt);
      }
    }

    m_isReady = true;
  } catch (const std::runtime_error& error) {
    std::cout << "Parsing Log File Failed: " << error.what();
  }
}

void Log::ParseLevelString(const String& levelStr) {
  std::vector<String> results;
  boost::split(results, levelStr, [](const char character) { return character == ' '; });

  for (const auto& logLevel : results) {
    const char index      = toupper(logLevel[0]);
    String level          = logLevel.substr(1, logLevel.length() - 1);
    const auto levelValue = U8(std::stoi(level));

    switch (index) {
      case 'D':
        m_debugLevel = levelValue;
        break;

      case 'I':
        m_infoLevel = levelValue;
        break;

      case 'W':
        m_warningLevel = levelValue;
        break;

      case 'E':
        m_errorLevel = levelValue;
        break;

      case 'F':
        m_fatalLevel = levelValue;
        break;

      default:
        std::cout << "Invalid Log Levels in Stream. Check log config file";
        break;
    }
  }
}

void Log::Debug(U32 level, const String& str) const {
  if (!m_isReady || level < m_debugLevel) {
    return;
  }

  BOOST_LOG_TRIVIAL(debug) << str.c_str();
}

void Log::Info(U32 level, const String& str) const {
  if (!m_isReady || level < m_infoLevel) {
    return;
  }

  BOOST_LOG_TRIVIAL(info) << str.c_str();
}


void Log::Warning(U32 level, const String& str) const {
  if (!m_isReady || level < m_warningLevel) {
    return;
  }

  BOOST_LOG_TRIVIAL(warning) << str.c_str();
}

void Log::Error(U32 level, const String& str) const {
  if (!m_isReady || level < m_errorLevel) {
    return;
  }

  BOOST_LOG_TRIVIAL(error) << str.c_str();
}

void Log::Fatal(U32 level, const String& str) const {
  if (!m_isReady || level < m_fatalLevel) {
    return;
  }

  BOOST_LOG_TRIVIAL(fatal) << str.c_str();
}
} // namespace Azura
