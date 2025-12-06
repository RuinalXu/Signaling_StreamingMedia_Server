#ifndef SIP_GB_PLAY_H
#define SIP_GB_PLAY_H

#include "SipTaskBase.h"

/**
 *  下级对上级INVITE请求的处理
 */
class SipGbPlay : public SipTaskBase {
public:
    typedef struct _MediaInfo {
        std::string sessionName;
        std::string sdp_protol = "RTP/AVP";
        std::string dstRtpAddr;
        int dstRtpPort;
        std::string setUp = "";
        int startTime = 0;
        int endTime = 0;
        int localRtpPort = 0;
    } MediaInfo;
public:
    SipGbPlay();
    ~SipGbPlay();
    virtual void run(pjsip_rx_data *rdata);

    void dealWithInvite(pjsip_rx_data *rdata);
    void dealWithBye(pjsip_rx_data *rdata);

    void resWithSdp(pjsip_rx_data *rdata, int status_code, std::string devid, MediaInfo sdpInfo);

    static void onStateChanged(pjsip_inv_session *inv, pjsip_event *e);
    static void onNewSession(pjsip_inv_session *inv, pjsip_event *e);
    static void onMediaUpdate(pjsip_inv_session *inv_ses, pj_status_t status);
    void onSendAck(pjsip_inv_session* inv, pjsip_rx_data* rdata);
};

#endif
