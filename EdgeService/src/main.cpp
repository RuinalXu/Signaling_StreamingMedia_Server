#include <fmt/base.h>
#include <log/LogManager.h>
#include <signal.h>
#include "SipLocalConfig.h"
#include "GlobalCtl.h"
#include "SipRegister.h"
#include "SipHeartBeat.h"


void* func(void* argc) {
	pthread_t id = pthread_self();
	LOG(INFO) << "current thread id:"<<id;
	return NULL;
}

int main(int argc, char* argv[]) {
    GlogInitializer glogInit(0);

    // 忽略control+c的信号
    // signal(SIGINT, SIG_IGN);

	// 本地配置信息初始化
	LOG(INFO) << "[EdgeService] local config init ...";
    SipLocalConfig* config = new SipLocalConfig();
    int ret = config->ReadConf();
    if (ret == -1) {
		LOG(ERROR) << "read config error";
		return ret;
	}

	// pjsip核心对象初始化
	LOG(INFO) << "[EdgeService] sip init ...";
    bool re = GlobalCtl::instance()->init(config);
	if (re == false) {
		LOG(ERROR) << "init error";
		return -1;
	}

	// 查看主线程信息
    pthread_t pid;
	ret = EC::ECThread::createThread(func, NULL, pid);
	if (ret != 0) {
		ret = -1;
		LOG(ERROR)<<"create thread error";
		return ret;
	}
	LOG(INFO) << "create thread pid:" << pid;
	LOG(INFO) << "main thread pid:" << pthread_self();

	// 启动定时注册服务
	LOG(INFO) << "[EdgeService] start sip register ...";
	SipRegister* regc = new SipRegister();
	regc->registerServiceStart();

	// 启动定时心跳包的发送
	LOG(INFO) << "[EdgeService] sip heart beat send ...";
	SipHeartBeat* heart = new SipHeartBeat();
	heart->gbHeartBeatServiceStart();

	while(true) {
		sleep(30);
	}
    return 0;
}