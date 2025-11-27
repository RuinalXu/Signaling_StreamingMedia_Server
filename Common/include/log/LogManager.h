#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <filesystem>
#include <glog/logging.h>

#define LOG_DIR "./logs"
#define LOG_FILE_NAME "CentralServer.log"

/**
 *  glog初始化
 */
class GlogInitializer {
public:
    explicit GlogInitializer(const int type) {
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
    }
    ~GlogInitializer(){
        google::ShutdownGoogleLogging();
    }
};

#endif
