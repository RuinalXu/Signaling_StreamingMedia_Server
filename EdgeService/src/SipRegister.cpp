#include "SipRegister.h"
#include <sip/SipMessageHeader.h>

SipRegister::SipRegister() {
    m_regTimer = new TaskTimer(3);
}

SipRegister::~SipRegister() {
    if (m_regTimer) {
        delete m_regTimer;
        m_regTimer = NULL;
    }
}

static void client_cb(struct pjsip_regc_cbparam *param) {
    LOG(INFO) << "code:" << param->code;
    if (param->code == 200) {
        GlobalCtl::SupDomainInfo* subinfo = (GlobalCtl::SupDomainInfo*)param->token;
        subinfo->registered =true;
    }
    return;
}

/**
 *  设置定时任务的入口函数指针和参数
 */
void SipRegister::RegisterProc(void* param){
    SipRegister* pthis = (SipRegister*)param;
    // 定义智能锁,并使用全局锁进行初始化
    AutoMutexLock lock(&(GlobalCtl::globalLock));
    GlobalCtl::SUPDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSupDomainInfoList().begin();
    for(;iter != GlobalCtl::instance()->getSupDomainInfoList().end(); iter++) {
        if (!(iter->registered)) {
            if (pthis->gbRegister(*iter)<0) {
                LOG(ERROR) << "register error for:" << iter->sipId;
            }
        }
    }
}

/**
 *  对外提供的注册接口
 */
void SipRegister::registerServiceStart() {
    if (m_regTimer) {
        m_regTimer->setTimerFun(SipRegister::RegisterProc, (void*)this);
        m_regTimer->start();
    }
}

/**
 *  基于GB2881发送的注册请求
 */
int SipRegister::gbRegister(GlobalCtl::SupDomainInfo& node) {
    // 封装sip message
    SipMessage msg;
    msg.setFrom((char*)GBOJ(gConfig)->sipId().c_str(), (char*)GBOJ(gConfig)->sipIp().c_str());
    msg.setTo((char*)GBOJ(gConfig)->sipId().c_str(), (char*)GBOJ(gConfig)->sipIp().c_str());
    if(node.protocal == 1) {
        msg.setUrl((char*)node.sipId.c_str(), (char*)node.addrIp.c_str(), node.sipPort, (char*)"tcp");
    } else {
        msg.setUrl((char*)node.sipId.c_str(), (char*)node.addrIp.c_str(), node.sipPort);
    }
    msg.setContact((char*)GBOJ(gConfig)->sipId().c_str(), (char*)GBOJ(gConfig)->sipIp().c_str(), GBOJ(gConfig)->sipPort());

    pj_str_t from = pj_str(msg.FromHeader());
    pj_str_t to = pj_str(msg.ToHeader());
    pj_str_t line = pj_str(msg.RequestUrl());
    pj_str_t contact = pj_str(msg.Contact());

    pj_status_t status = PJ_SUCCESS;
    do {
        pjsip_regc* regc;
        status = pjsip_regc_create(GBOJ(gSipServer)->GetEndPoint(), &node,&client_cb,&regc);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "pjsip_regc_create error";
            break;
        }

        status = pjsip_regc_init(regc, &line, &from, &to, 1, &contact, node.expires);
        if (PJ_SUCCESS != status) {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_init error";
            break;
        }

        pjsip_tx_data* tdata = NULL;
        status = pjsip_regc_register(regc, PJ_TRUE, &tdata);
        if (PJ_SUCCESS != status) {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_register error";
            break;
        }

        status = pjsip_regc_send(regc, tdata);
        if (PJ_SUCCESS != status) {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_send error";
            break;
        }
    } while(0);

    int ret = 0;
    if (PJ_SUCCESS != status) {
        ret = -1;
    }
    return ret;
}