#ifndef SIP_GB_PLAY_H
#define SIP_GB_PLAY_H

#include <sip/SipInclude.h>

class SipGbPlay {
public:
    SipGbPlay();
    ~SipGbPlay();

    static void onStateChanged(pjsip_inv_session *inv, pjsip_event *e);
    static void onNewSession(pjsip_inv_session *inv, pjsip_event *e);
    static void onMediaUpdate(pjsip_inv_session *inv_ses, pj_status_t status);
    static void onSendAck(pjsip_inv_session *inv, pjsip_rx_data *rdata);
};

#endif