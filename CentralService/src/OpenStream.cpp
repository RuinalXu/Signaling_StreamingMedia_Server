#include "OpenStream.h"
#include <sip/SipInclude.h>
#include <sip/SipMessageHeader.h>
#include <log/LogManager.h>
#include "GlobalCtl.h"

/* rtp负载类型定义 */
static string rtpmap_ps = "96 PS/90000";

OpenStream::OpenStream() {
    m_pStreamTimer = new TaskTimer(600);
}

OpenStream::~OpenStream() {}

void OpenStream::StreamServiceStart() {
    if (m_pStreamTimer && m_pCheckSessionTimer) {
        m_pStreamTimer->setTimerFun(OpenStream::StreamGetProc, this);
        m_pStreamTimer->start();
    }
}

/**
 *  实现定时器任务
 */
void OpenStream::StreamGetProc(void* param) {
    /* 判断设备信息是否存在 */
    // 通过推送设备目录树中的DeviceID判断
    LOG(INFO) << "GlobalCtl::gRcvIpc:" << GlobalCtl::gRcvIpc;
    if(!GlobalCtl::gRcvIpc) {
        return;
    }
    LOG(INFO) << "start stream";
    // 这一部分实际上是上级客户端发送过的信息,这里我们直接使用定值
    DeviceInfo info;
    info.devid = "11000000001310000059";
    info.playformId = "11000000002000000001";
    info.streamName = "Play";
    info.startTime = 0;
    info.endTime = 0;
    info.protocal = 0;
    // info.setupType = "passive";

    /* 发送INVITE请求 */
    SipMessage msg;
    // 智能锁缩小栈空间,实际的栈空间就{}之间的部分
    {
        AutoMutexLock lock(&(GlobalCtl::globalLock));
        GlobalCtl::SUBDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSubDomainInfoList().begin();
        for(;iter != GlobalCtl::instance()->getSubDomainInfoList().end(); iter++) {
            if(iter->sipId == info.playformId) {
                msg.setFrom((char*)GBOJ(gConfig)->sipId().c_str(),(char*)GBOJ(gConfig)->sipIp().c_str());
                msg.setTo((char*)iter->sipId.c_str(),(char*)iter->addrIp.c_str());
                msg.setUrl((char*)iter->sipId.c_str(),(char*)iter->addrIp.c_str(),iter->sipPort);
                msg.setContact((char*)GBOJ(gConfig)->sipId().c_str(),(char*)GBOJ(gConfig)->sipIp().c_str(),GBOJ(gConfig)->sipPort());
            }
        }
    }
    pj_str_t from = pj_str(msg.FromHeader());
    pj_str_t to = pj_str(msg.ToHeader());
    pj_str_t contact = pj_str(msg.Contact());
    pj_str_t requestUrl = pj_str(msg.RequestUrl());
    
    pjsip_dialog* dlg;
    pj_status_t status = pjsip_dlg_create_uac(pjsip_ua_instance(), &from, &contact, &to, &requestUrl, &dlg);
    if(PJ_SUCCESS != status) {
        LOG(ERROR) << "pjsip_dlg_create_uac ERROR";
        return;
    }

    /* 组织SDP部分 */
    pjmedia_sdp_session* sdp = NULL;
    sdp = (pjmedia_sdp_session*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(), sizeof(pjmedia_sdp_session));
    // Session Description Protocol Version (v): 0
    sdp->origin.version = 0;
    // Owner/Creator, Session Id (o): 33010602001310019325 0 0 IN IP4 10.64.49.44
    sdp->origin.user = pj_str((char*)info.devid.c_str());
    sdp->origin.id = 0;
    sdp->origin.net_type = pj_str("IN");
    sdp->origin.addr_type = pj_str("IP4");
    sdp->origin.addr = pj_str((char*)GBOJ(gConfig)->sipIp().c_str());
    // Session Name (s): Play
    sdp->name = pj_str((char*)info.streamName.c_str());
    // Connection Information (c): IN IP4 10.64.49.218
    sdp->conn = (pjmedia_sdp_conn*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(),sizeof(pjmedia_sdp_conn));
    sdp->conn->net_type = pj_str("IN");
    sdp->conn->addr_type = pj_str("IP4");
    sdp->conn->addr = pj_str((char*)GBOJ(gConfig)->sipIp().c_str());
    // Time Description, active time (t): 0 0
    sdp->time.start = info.startTime;
    sdp->time.stop = info.endTime;
    // Media Description, name and address (m): video 5514 RTP/AVP 96
    sdp->media_count = 1;
    pjmedia_sdp_media* m = (pjmedia_sdp_media*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(), sizeof(pjmedia_sdp_media));
    sdp->media[0] = m;
    m->desc.media = pj_str("video");
    m->desc.port = 20000;
    m->desc.port_count = 1;
    if(info.protocal) {
        m->desc.transport = pj_str("TCP/RTP/AVP");
    } else {
        m->desc.transport = pj_str("RTP/AVP");
    }
    // 指定PS流
    m->desc.fmt_count = 1;
    m->desc.fmt[0] = pj_str("96");

    /*
        Media Attribute (a): rtpmap:96 PS/90000
        Media Attribute (a): recvonly
        Media Attribute (a): setup:active
    */
    m->attr_count = 0;
    pjmedia_sdp_attr* attr = (pjmedia_sdp_attr*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(), sizeof(pjmedia_sdp_attr));
    attr->name = pj_str("rtpmap");
    attr->value = pj_str((char*)rtpmap_ps.c_str());
    m->attr[m->attr_count++] = attr;

    attr = (pjmedia_sdp_attr*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(), sizeof(pjmedia_sdp_attr));
    attr->name = pj_str("recvonly");
    m->attr[m->attr_count++] = attr;
    if(info.protocal) {
        attr = (pjmedia_sdp_attr*)pj_pool_zalloc(GBOJ(gSipServer)->GetPool(), sizeof(pjmedia_sdp_attr));
        attr->name = pj_str("setup");
        attr->value = pj_str((char*)info.setupType.c_str());
        m->attr[m->attr_count++] = attr;
    }

    /* 为dialog中指定的对话框创建UAC的一个邀请对话,也就是INVITE会话创建 */
    pjsip_inv_session* inv;
    status = pjsip_inv_create_uac(dlg, sdp, 0, &inv);
    if(PJ_SUCCESS != status) {
        pjsip_dlg_terminate(dlg);
        LOG(ERROR) << "pjsip_inv_create_uac ERROR";
        return;
    }

    /* 使用INVITE请求初始化 */
    pjsip_tx_data* tdata;
    status = pjsip_inv_invite(inv, &tdata);
    if(PJ_SUCCESS != status)
    {
        pjsip_dlg_terminate(dlg);
        LOG(ERROR)<<"pjsip_inv_invite ERROR";
        return;
    }
    // 组织SIP Message Body中subject字段
    pj_str_t subjectName = pj_str("Subject");
    char subjectBuf[128] = {0};
    sprintf(subjectBuf, "%s:0,%s:0", info.devid.c_str(), GBOJ(gConfig)->sipId().c_str());
    pj_str_t subjectValue = pj_str(subjectBuf);
    pjsip_generic_string_hdr* hdr = pjsip_generic_string_hdr_create(GBOJ(gSipServer)->GetPool(), &subjectName, &subjectValue);
    pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)hdr);

    // 发送INVITE消息
    status = pjsip_inv_send_msg(inv,tdata);
    if(PJ_SUCCESS != status) {
        pjsip_dlg_terminate(dlg);
        LOG(ERROR) << "pjsip_inv_send_msg ERROR";
        return;
    }

    GlobalCtl::gRcvIpc = false;
    return;
    /* 引用GbSeession */
    // Gb28181Session* session = new Gb28181Session();
    
}