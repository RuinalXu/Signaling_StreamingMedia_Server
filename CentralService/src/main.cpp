#include <fmt/base.h>

#include "common.h"
#include "log_manager.h"
#include "sip_local_config.h"
#include "global_controller.h"
#include "EC_thread.h"

using namespace embedded_controller;


void* func(void* argc) {
    pthread_t id = pthread_self();
    LOG(INFO) << "current thread id:" << id;
}

int main(int argc, char ** argv) {
    fmt::print("Hello SipSupService\n");

    // 忽略control+c的信号
    signal(SIGINT, SIG_IGN);
    
    loger::GlogInitializer glogInit(0);
    
    SipLocalConfig* config = new SipLocalConfig();
    int ret = config -> readConfig();
    if (ret == -1) {
        LOG(ERROR) << "read config error";
        return ret;
    }
    bool re = GlobalController::instance() -> init(config);
    if (re == false) {
        LOG(ERROR) << "init error";
        return -1;
    }
    LOG(INFO) << GBOJ(g_config) -> localIp();

    // 使用EC_thread.cpp
    pthread_t pid;
    embedded_controller::ECThread::createThread(func, NULL, pid);
    if (ret != 0) {
        ret = -1;
        LOG(ERROR) << "create thread error";
        return ret;
    }
    LOG(INFO) << "create thread pid:" << pid;
    LOG(INFO) << "main thread pid:" << pthread_self();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
    
    return 0;
}