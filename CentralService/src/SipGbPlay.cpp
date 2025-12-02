#include "SipGbPlay.h"
#include <log/LogManager.h>

SipGbPlay::SipGbPlay() {}

SipGbPlay::~SipGbPlay() {}

/**
 *  pj_media模块注册的时,需要强制定义的回调之一
 */
void SipGbPlay::onStateChanged(pjsip_inv_session* inv, pjsip_event* e) {

}

/**
 *  pj_media模块注册的时,需要强制定义的回调之一
 */
void SipGbPlay::onNewSession(pjsip_inv_session* inv, pjsip_event* e) {

}

/**
 *  下级携带SDP发送MESSAGE请求的时候,上级判断MESSAGE包中的body部分若有SDP信息则会回调此函数
 *  在此回调函数中上级实现解析对下级发送的SDP协议的解析,关于RTP会话的创建/连接都在此回调函数中完成
 */
void SipGbPlay::onMediaUpdate(pjsip_inv_session* inv_ses, pj_status_t status) {
    LOG(INFO) << "onMediaUpdate";
    if (inv_ses == NULL) {
        return;
    }
    
    /* SDP解析 */
    pjsip_tx_data* tdata;
    const pjmedia_sdp_session* remote_sdp = NULL;
    // 获取sdp session的句柄
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

    /* RTP会话 */
    
    // 进行RTP的Session的创建

}

void SipGbPlay::onSendAck(pjsip_inv_session* inv, pjsip_rx_data* rdata) {

}