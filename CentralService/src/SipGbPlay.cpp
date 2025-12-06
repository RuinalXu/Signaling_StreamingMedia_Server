#include "SipGbPlay.h"
#include <log/LogManager.h>
#include <sip/SipInclude.h>

SipGbPlay::SipGbPlay() {}

SipGbPlay::~SipGbPlay() {}

/**
 *  pj_media模块注册的时,需要强制定义的回调之一
 */
void SipGbPlay::onStateChanged(pjsip_inv_session* inv, pjsip_event* e) {
    LOG(INFO) << "SipGbPlay::onStateChanged";
}

/**
 *  pj_media模块注册的时,需要强制定义的回调之一
 */
void SipGbPlay::onNewSession(pjsip_inv_session* inv, pjsip_event* e) {
    LOG(INFO) << "SipGbPlay::onNewSession";
}

/**
 *  当下级发送MESSAGE请求200 OK并携带SDP的时候,触发此回调函数
 *  PJSIP底层判断SIP MESSAGE的body部分为是否为SDP信息,从而判断是否触发此回调
 *  此回调函数: (1)上级实现解析对下级发送的SDP协议的解析; (2)关于RTP会话的创建/连接都在此回调函数中完成
 */
void SipGbPlay::onMediaUpdate(pjsip_inv_session* inv_ses, pj_status_t status) {
    LOG(INFO) << "SipGbPlay::onMediaUpdate";
    if (inv_ses == NULL) {
        return;
    }
    
    /* 解析下级发送给上级的SDP内容 */
    pjsip_tx_data* tdata;
    const pjmedia_sdp_session* remote_sdp = NULL;
    /* 获取SDP session的句柄 */
    pjmedia_sdp_neg_get_active_remote(inv_ses->neg, &remote_sdp);
    if (remote_sdp == NULL) {
        pjsip_inv_end_session(inv_ses, PJSIP_SC_UNSUPPORTED_MEDIA_TYPE, NULL, &tdata);
        pjsip_inv_send_msg(inv_ses, tdata);
        return;
    }
    if ((remote_sdp->media_count <= 0) || remote_sdp->media[remote_sdp->media_count - 1] == NULL) {
        pjsip_inv_end_session(inv_ses, PJSIP_SC_FORBIDDEN, NULL, &tdata);
        pjsip_inv_send_msg(inv_ses, tdata);
        return;
    }
    pjmedia_sdp_media* media = remote_sdp->media[remote_sdp->media_count - 1];
    int sdp_port = media->desc.port;
    pjmedia_sdp_conn* connect = remote_sdp->conn;
    std::string ip(connect->addr.ptr, connect->addr.slen);
    LOG(INFO) << "remote rtp ip:" << ip << " remote rtp port:" << sdp_port;
}

void SipGbPlay::onSendAck(pjsip_inv_session* inv, pjsip_rx_data* rdata) {

}