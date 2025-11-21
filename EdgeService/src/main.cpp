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

	LOG(ERROR) << "111111111111";
    bool re = GlobalCtl::instance()->init(config);
	LOG(ERROR) << "22222222222";
	if (re == false) {
		LOG(ERROR) << "3333333";
		LOG(ERROR) << "init error";
		return -1;
	}
	LOG(ERROR) << "44444444";

    pthread_t pid;
	ret = EC::ECThread::createThread(func, NULL, pid);
	if (ret != 0) {
		ret = -1;
		LOG(ERROR)<<"create thread error";
		return ret;
	}
	LOG(INFO) << "create thread pid:" <<pid;
	LOG(INFO) << "main thread pid:" <<pthread_self();

	SipRegister* regc = new SipRegister();
    
	while(true) {
		sleep(30);
	}
    return 0;
}