#ifndef GLOBAL_CONTROLLER_H
#define GLOBAL_CONTROLLER_H

#include "common.h"
#include "SipLocalConfig.h"
#include "ThreadPool.h"
#include "SipCore.h"

class  GlobalCtl;
#define GBOJ(obj) GlobalCtl::instance() -> obj

/**
 * 全局类： 管理下级平台及设备信息、线程池对象，以及sipserver对象；
 * 对外只提供一个类实例，因此需要实现单例模型；
 */

class GlobalCtl {
private:
    GlobalCtl(){};   
    ~GlobalCtl();
    GlobalCtl(const GlobalCtl& g);
    const GlobalCtl& operator=(const GlobalCtl& g);

    static GlobalCtl* m_pInstance;
public:
    SipLocalConfig* gConfig;

    static GlobalCtl* instance();

    bool init(void* param);

    ThreadPool* gThreadPool = NULL;

    /**
     * pjsip核心组件初始化
     */
    SipCore* gSipServer = NULL;
};

#endif