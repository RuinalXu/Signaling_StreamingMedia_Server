#include <fmt/base.h>
#include "LogManager.h"
#include "SipLocalConfig.h"
#include "GlobalCtl.h"


int main(int argc, char* argv[]) {
    GlogInitializer glogInit(0);

    // 忽略control+c的信号
    signal(SIGINT, SIG_IGN);

    SipLocalConfig* config = new SipLocalConfig();
    int ret = config->ReadConf();
	if (ret == -1) {
		LOG(ERROR) << "read config error";
		return ret;
	}

	bool re = GlobalCtl::instance()->init(config);
	if (re == false) {
		LOG(ERROR) << "init error";
		return -1;
	}

	while(true)
	{
		sleep(30);
	}

    
    return 0;
}