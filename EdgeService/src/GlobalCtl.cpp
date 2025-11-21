#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;
bool GlobalCtl::gStopPool = false;


GlobalCtl* GlobalCtl::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalCtl();
    }
    return m_pInstance;
}

bool GlobalCtl::init(void* param) {
    gConfig = (SipLocalConfig*)param;
    if (!gConfig) {
        return false;
    }

    if (!gThPool) {
        gThPool = new ThreadPool();
        gThPool->createThreadPool(10);
    }

    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer->InitSip(gConfig->sipPort());

    return true;
}

