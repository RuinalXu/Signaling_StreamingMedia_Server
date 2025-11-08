#include "GlobalController.h"

GlobalController* GlobalController::m_pInstance = NULL;

GlobalController* GlobalController::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalController();
    }
    return m_pInstance;
}

bool GlobalController::init(void* param) {
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
