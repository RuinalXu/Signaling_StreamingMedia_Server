#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include <sip/SipDef.h>
#include <timer/TaskTimer.h>
#include "SipTaskBase.h"

/**
 *  上级处理注册相关业务
 */
class SipRegister : public SipTaskBase {
private:
    /**
     *  上级定时对已注册下级的生存期进行检查
     */
    TaskTimer* m_regTimer;
public:
    SipRegister();
    ~SipRegister();

    virtual pj_status_t run(pjsip_rx_data* rdata);
    pj_status_t registerRequestMessage(pjsip_rx_data *rdata);
    pj_status_t dealWithRegister(pjsip_rx_data* rdata);
    pj_status_t dealWithAuthorRegister(pjsip_rx_data *rdata);
    void registerServiceStart();
    static void registerCheckProc(void* param);
};


#endif
