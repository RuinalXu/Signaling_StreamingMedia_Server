#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include <timer/TaskTimer.h>
#include "GlobalCtl.h"

/**
 *  下级注册请求业务
 */
class SipRegister {
private:
    /**
     *  定时任务指针
     */
    TaskTimer* m_regTimer;
public:
    SipRegister();
    ~SipRegister();

    void registerServiceStart();
    int gbRegister(GlobalCtl::SupDomainInfo& node);
    static void RegisterProc(void* param);
};

#endif
