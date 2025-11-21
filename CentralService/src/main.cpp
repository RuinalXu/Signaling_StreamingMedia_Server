#include <fmt/base.h>
#include "LogManager.h"
#include "SipLocalConfig.h"


int main(int argc, char* argv[]) {
    GlogInitializer glogInit(0);

    // 忽略control+c的信号
    signal(SIGINT, SIG_IGN);

    SipLocalConfig* config = new SipLocalConfig();
    int ret = config->ReadConf();
    fmt::println("ret = {}", ret);

    
    return 0;
}