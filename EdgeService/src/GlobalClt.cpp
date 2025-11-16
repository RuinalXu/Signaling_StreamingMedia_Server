#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;

GlobalCtl* GlobalCtl::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalCtl();
    }
    return m_pInstance;
}

bool GlobalCtl::init(void* param) {
    gConfig = (SipLocalConfig*) param;
    if (!gConfig) {
        return false;
    }

    if (!gThreadPool) {
        gThreadPool = new ThreadPool();
        gThreadPool -> createThreadPool(10);
    }

    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer -> initSip(gConfig -> sipPort())

    return true;
}
