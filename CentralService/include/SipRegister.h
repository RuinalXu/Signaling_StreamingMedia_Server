#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include "SipTaskBase.h"
#include "Common.h"
#include "SipDef.h"

/**
 *  上级处理注册相关业务
 */
class SipRegister : public SipTaskBase {
public:
    SipRegister();
    ~SipRegister();

    virtual pj_status_t run(pjsip_rx_data* rdata);
    pj_status_t RegisterRequestMessage(pjsip_rx_data *rdata);
    pj_status_t dealWithRegister(pjsip_rx_data* rdata);
    pj_status_t dealWithAuthorRegister(pjsip_rx_data *rdata);
};


#endif
