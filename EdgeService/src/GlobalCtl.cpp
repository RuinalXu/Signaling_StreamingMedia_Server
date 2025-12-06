#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;
GlobalCtl::SUPDOMAININFOLIST GlobalCtl::supDomainInfoList;
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
    
    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer->InitSip(gConfig->sipPort());

    return true;
}

DevTypeCode GlobalCtl::getSipDevInfo(string id) {
    DevTypeCode code_type = Error_code;
    string tmp = id.substr(10, 3);
    // atio()标准库函数: 字符串转int值
    int type = atoi(tmp.c_str());

    switch(type) {
        case Camera_Code:
            code_type = Camera_Code;
            break;
        case Ipc_Code:
            code_type = Ipc_Code;
            break;   
        default:
            code_type = Error_code;
            break;
    }
    return code_type;
}

