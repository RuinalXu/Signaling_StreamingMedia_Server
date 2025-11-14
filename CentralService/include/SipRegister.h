#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include "SipTaskBase.h"

class SipRegister : public SipTaskBase{
public:
    SipRegister();
    ~SipRegister();

    virtual pj_status_t run(pjsip_rx_data *rdata);
    pj_status_t RegisterRequestMessage(pjsip_rx_data *rdata);
};


#endif

