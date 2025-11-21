#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;
bool GlobalCtl::gStopPool = false;
GlobalCtl::SUPDOMAININFOLIST GlobalCtl::supDomainInfoList;


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

    // 当本地配置初始化成功后进行supDomainInfoList的初始化
    SupDomainInfo info;
    auto iter = gConfig->supNodeInfoList.begin();
    for (;iter != gConfig->supNodeInfoList.end(); ++iter) {
        info.sipId = iter->id;
        info.addrIp = iter->ip;
        info.sipPort = iter->port;
        info.protocal = iter->poto;
        info.expires = iter->expires;
        if (iter->auth) {
            info.isAuth = (iter->auth = 1) ? true : false;
            info.usr = iter->usr;
            info.pwd = iter->pwd;
            info.realm = iter->realm;
        }
        supDomainInfoList.push_back(info);
    }
    LOG(INFO)<<"supDomainInfoList.SIZE:"<<supDomainInfoList.size();

    if (!gThPool) {
        gThPool = new ThreadPool();
        gThPool->createThreadPool(10);
    }

    LOG(ERROR) << "222";
    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer->InitSip(gConfig->sipPort());

    return true;
}

