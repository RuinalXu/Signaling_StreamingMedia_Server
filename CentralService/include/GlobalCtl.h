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

/**
 *  pjlib的线程注册
 */
static pj_status_t pjcall_thread_register(pj_thread_desc& desc) {
    pj_thread_t* tread = 0;
    if (!pj_thread_is_registered()) {
        return pj_thread_register(NULL, desc, &tread);
    }
    return PJ_SUCCESS;
}

class GlobalCtl {
public:
    typedef struct _SubDomainInfo {
        _SubDomainInfo() {
            sipId = "";
            addrIp = "";
            sipPort = 0;
            protocal = 0;
            registered = 0;
            expires = 0;
            lastRegTime = 0;
            auth = false;
        }
        bool operator==(string id) {
            return (this->sipId == id);
        }
        string sipId;
        string addrIp;
        int sipPort;
        int protocal;
        int registered;
        int expires;
        time_t lastRegTime;
        bool auth;
    } SubDomainInfo;
    typedef list<SubDomainInfo> SUBDOMAININFOLIST;
    SUBDOMAININFOLIST& getSubDomainInfoList() {
        return subDomainInfoList;
    }
public:
    SipLocalConfig* gConfig = NULL;
    ThreadPool* gThPool = NULL;
    SipCore* gSipServer = NULL;

    /**
     *  控制轮询线程的退出
     */
    static bool gStopPoll;

    /**
     * 全局锁,为了使用智能锁从而将其可见性设置为public
     */
    static pthread_mutex_t globalLock;
public:
    static GlobalCtl* instance();
    bool init(void* param);

    /**
     *  提供全局锁的获取锁和释放锁的接口
     */
    static void get_global_mutex() {
        pthread_mutex_lock(&globalLock);
    }
    static void free_global_mutex() {
        pthread_mutex_unlock(&globalLock);
    }
    // static bool checkIsExist(string id);
    // static bool checkIsVaild(string id);
    // static void setExpires(string id,int expires);
    // static void setRegister(string id,bool registered);
    // static void setLastRegTime(string id,time_t t);
    // static bool getAuth(string id);
private:
    static GlobalCtl* m_pInstance;
    static SUBDOMAININFOLIST subDomainInfoList;
private:
    GlobalCtl(){}
    ~GlobalCtl(){}
    GlobalCtl(const GlobalCtl& global);
    const GlobalCtl& operator=(const GlobalCtl& global);
};

#endif
