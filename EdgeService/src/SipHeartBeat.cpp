#include "SipHeartBeat.h"
#include "SipMessageHeader.h"

SipHeartBeat::SipHeartBeat() {
    m_heartTimer = new TaskTimer(10);
}

SipHeartBeat::~SipHeartBeat() {
    if (m_heartTimer) {
        delete m_heartTimer;
        m_heartTimer = NULL;
    }
}

void SipHeartBeat::HeartBeatProc(void* param) {
    SipHeartBeat* pthis = (SipHeartBeat*)param;
    AutoMutexLock lock(&(GlobalCtl::globalLock));
    GlobalCtl::SUPDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSupDomainInfoList().begin();
    for (; iter != GlobalCtl::instance()->getSupDomainInfoList().end(); iter++) {
        if (iter->registered) {
            if (pthis->gbHeartBeat(*iter)<0) {
                LOG(ERROR) << "keep alive error for:" << iter->sipId;
            }
        }
    }
}

void SipHeartBeat::gbHeartBeatServiceStart() {
    if (m_heartTimer) {
        m_heartTimer->setTimerFun(HeartBeatProc, this);
        m_heartTimer->start();
    }
}

int SipHeartBeat::gbHeartBeat(GlobalCtl::SupDomainInfo& node) {
    // 封装消息
    SipMessage msg;
    msg.setFrom((char*)GBOJ(gConfig)->sipId().c_str(),(char*)GBOJ(gConfig)->sipIp().c_str());
    msg.setTo((char*)node.sipId.c_str(),(char*)node.addrIp.c_str());
    msg.setUrl((char*)node.sipId.c_str(),(char*)node.addrIp.c_str(),node.sipPort);
    pj_str_t from = pj_str(msg.FromHeader());
    pj_str_t to = pj_str(msg.ToHeader());
    pj_str_t line = pj_str(msg.RequestUrl());

    string method = "MESSAGE";
    pjsip_method reqMethod = {PJSIP_OTHER_METHOD, {(char*)method.c_str(), method.length()}};
    pjsip_tx_data* tdata;

}