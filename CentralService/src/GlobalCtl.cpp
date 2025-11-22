#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;
GlobalCtl::SUBDOMAININFOLIST GlobalCtl::subDomainInfoList;
pthread_mutex_t GlobalCtl::globalLock = PTHREAD_MUTEX_INITIALIZER;
bool GlobalCtl::gStopPoll = false;

GlobalCtl* GlobalCtl::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalCtl();
    }
    return m_pInstance;
}

/**
 *  初始化函数,配置信息,endpoint对象
 */
bool GlobalCtl::init(void* param) {
    gConfig = (SipLocalConfig*)param;
    if (!gConfig) {
        return false;
    }

    // 当本地配置初始化成功后进行supDomainInfoList的初始化
    SubDomainInfo info;
    auto iter = gConfig->subNodeInfoList.begin();
    for (;iter != gConfig->subNodeInfoList.end(); ++iter) {
        info.sipId = iter->id;
        info.addrIp = iter->ip;
        info.sipPort = iter->port;
        info.protocal = iter->poto;
        info.auth = iter->auth;
        subDomainInfoList.push_back(info);
    }
    LOG(INFO)<<"subDomainInfoList.SIZE:"<<subDomainInfoList.size();

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

/**
 *  查询下级节点是否在列表中
 */
 bool GlobalCtl::checkIsExist(string id) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        return true;
    }
    return false;
 }
/**
 *  设置下级的生存周期
 */
 void GlobalCtl::setExpires(string id, int expires) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        it->expires = expires;
    }
 }

