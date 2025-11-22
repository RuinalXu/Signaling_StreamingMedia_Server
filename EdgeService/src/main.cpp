#include <fmt/base.h>
#include "LogManager.h"
#include "SipLocalConfig.h"
#include "GlobalCtl.h"
#include "SipRegister.h"


void* func(void* argc) {
	pthread_t id = pthread_self();
	LOG(INFO) << "current thread id:"<<id;
	return NULL;
}

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

    pthread_t pid;
	ret = EC::ECThread::createThread(func, NULL, pid);
	if (ret != 0) {
		ret = -1;
		LOG(ERROR)<<"create thread error";
		return ret;
	}
	LOG(INFO) << "create thread pid:" <<pid;
	LOG(INFO) << "main thread pid:" <<pthread_self();


	LOG(ERROR) << "TEST 111";

	// 启动定时注册服务
	SipRegister* regc = new SipRegister();
	regc->registerServiceStart();

	LOG(ERROR) << "main1111";
    
	while(true) {
		sleep(30);
	}
    return 0;
}