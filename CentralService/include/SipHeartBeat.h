#ifndef SIP_HEART_BEAT_H
#define SIP_HEART_BEAT_H

#include <sip/SipTaskBase.h>

/**
 *  上级处理下级发送过来的心跳包
 */
class SipHeartBeat : public SipTaskBase {
public:
    SipHeartBeat();
    ~SipHeartBeat();
    virtual pj_status_t run(pjsip_rx_data* rdata);
    pj_status_t HeartBeatMessage(pjsip_rx_data *rdata);
};

#endif
