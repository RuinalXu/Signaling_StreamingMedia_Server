#include <fmt/base.h>

// #include "common.h"
#include "LogManager.h"
// #include "SipLocalConfig.h"
// #include "GlobalCtl.h"
// #include "EC_thread.h"

// using namespace embedded_controller;


// void* func(void* argc) {
//     pthread_t id = pthread_self();
//     LOG(INFO) << "current thread id:" << id;
// }

int main(int argc, char ** argv) {
    GlogInitializer glogInit(0);

    // 忽略control+c的信号
    signal(SIGINT, SIG_IGN);

    LOG();
    
    return 0;
}