#ifndef GLOBAL_CTL_H
#define GLOBAL_CTL_H

#include <time.h>
#include <random>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>
#include "Common.h"
#include "SipLocalConfig.h"
#include "ThreadPool.h"
#include "SipCore.h"
#include "SipDef.h"


class GlobalCtl;
#define GBOJ(obj) GlobalCtl::instance()->obj


class GlobalCtl {
public:
    typedef struct _SupDomainInfo {
        _SupDomainInfo() {
            sipId = "";
            addrIp = "";
            sipPort = 0;
            protocal = 0;
            registered = 0;
            expires = 0;
            usr = "";
            pwd = "";
            isAuth = false;
            realm = "";
        }
        string sipId;
        string addrIp;
        int sipPort;
        int protocal;
        int registered;
        int expires;
        bool isAuth;
        string usr;
        string pwd; 
        string realm;
    } SupDomainInfo;
    typedef list<SupDomainInfo> SUPDOMAININFOLIST;
    SUPDOMAININFOLIST& getSupDomainInfoList() {
        return supDomainInfoList;
    }
public:
    static bool gStopPool;
    SipLocalConfig* gConfig = NULL;
    ThreadPool* gThPool = NULL;
    SipCore* gSipServer = NULL;
public:
    static GlobalCtl* instance();
    bool init(void* param);
private:
    static GlobalCtl* m_pInstance;
    static SUPDOMAININFOLIST supDomainInfoList;
private:
    GlobalCtl(){}
    ~GlobalCtl(){}
    GlobalCtl(const GlobalCtl& global);
    const GlobalCtl& operator=(const GlobalCtl& global);
};

#endif
