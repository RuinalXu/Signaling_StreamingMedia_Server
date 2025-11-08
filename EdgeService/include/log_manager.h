#ifndef LOGGER_H
#define LOGGER_H

#include <filesystem>
#include <string>
#include <signal.h>
#include <glog/logging.h>

#define LOG_DIR "./logs"
#define LOG_FILE_NAME "SipServer.log"


namespace loger {

class GlogInitializer {
public:
    GlogInitializer(const int type) {
        // 创建日志目录
        std::filesystem::create_directories(LOG_DIR);
        // 将日志重定向到指定文件中
        google::InitGoogleLogging(LOG_FILE_NAME);
        FLAGS_stderrthreshold = type;
        FLAGS_colorlogtostderr = true;
        FLAGS_logbufsecs = 0;
        FLAGS_log_dir = LOG_DIR;
        FLAGS_max_log_size = 4;
        google::SetLogDestination(google::GLOG_WARNING, "");
        google::SetLogDestination(google::GLOG_ERROR, "");
        signal(SIGPIPE, SIG_IGN);
    }
    ~GlogInitializer(){
        google::ShutdownGoogleLogging();
    }
};

// class SpdlogInitializer {
// public:
//     SpdlogInitializer() {
//     }
//     ~SpdlogInitializer() {
//     }
    // 用 glog
    // 在mian函数中初始化
    // logger::LoggerInit glogInit(0);
    // 在其他文件中使用
    // #include <glog/logging.h>

    // 用 spdlog
    // logger::SpdlogInit spdlogInit;        
    // 在其他文件中使用
    // #include <spdlog/spdlog.h>
    // spdlog::info("Hello from spdlog");
    // spdlog::warn("Warning message");
// };

}

#endif