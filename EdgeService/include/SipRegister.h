#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include "GlobalCtl.h"
#include "TaskTimer.h"

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
