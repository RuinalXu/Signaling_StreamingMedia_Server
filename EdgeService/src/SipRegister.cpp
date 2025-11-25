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

/**
 *  静态函数,但非成员静态函数
 */
static void client_cb(struct pjsip_regc_cbparam *param) {
    LOG(INFO) << "code:" << param->code;
    if (param->code == 200) {
        GlobalCtl::SupDomainInfo* subinfo = (GlobalCtl::SupDomainInfo*)param->token;
        subinfo->registered = true;
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

        if (node.isAuth) {
            pjsip_cred_info cred;
            pj_bzero(&cred, sizeof(pjsip_cred_info));
            cred.scheme = pj_str("digest");
            cred.realm = pj_str((char*)node.realm.c_str());
            cred.username = pj_str((char*)node.usr.c_str());
            cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
            cred.data = pj_str((char*)node.pwd.c_str());

            pjsip_regc_set_credentials(regc, 1, &cred);
            if (PJ_SUCCESS != status) {
                pjsip_regc_destroy(regc);
                LOG(ERROR) << "pjsip_regc_set_credentials error";
                break;
            }
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