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
#define GBOJ(obj) GlobalCtl::instance->obj


class GlobalCtl {
private:
    static GlobalCtl* m_pInstance;
private:
    GlobalCtl(){}
    ~GlobalCtl(){}
    GlobalCtl(const GlobalCtl& global);
    const GlobalCtl& operator=(const GlobalCtl& global);
public:
    SipLocalConfig* gConfig = NULL;
    ThreadPool* gThPool = NULL;
    SipCore* gSipServer = NULL;
public:
    static GlobalCtl* instance();
    bool init(void* param);

};

#endif
