#include "SipRegister.h"
#include "GlobalCtl.h"
#include <time.h>
#include <sys/sysinfo.h>
#include <sip/SipStatusCode.h>
#include <sip/SipTaskBase.h>

static pj_status_t auth_cred_callback(pj_pool_t *pool,
					    const pj_str_t *realm,
					    const pj_str_t *acc_name,
					    pjsip_cred_info *cred_info ) {
    pj_str_t usr = pj_str((char*)GBOJ(gConfig)->usr().c_str());
    if (pj_stricmp(acc_name,&usr) != 0) {
        LOG(ERROR)<<"usr name wrong";
        return PJ_FALSE;
    }
    pj_str_t pwd = pj_str((char*)GBOJ(gConfig)->pwd().c_str());
    cred_info->realm = *realm;
    cred_info->username = usr;
    cred_info->data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cred_info->data = pwd;
    return PJ_SUCCESS;
}

SipRegister::SipRegister() {
    m_regTimer = new TaskTimer(10);
}

SipRegister::~SipRegister() {
    LOG(INFO) << "~SipRegister";
    if (m_regTimer) {
        delete m_regTimer;
        m_regTimer = NULL;
    }
}

void SipRegister::registerServiceStart() {
    if (m_regTimer) {
        m_regTimer->setTimerFun(registerCheckProc, this);
        m_regTimer->start();
    }
}

/**
 *  上级定时检查已注册下级的生存期
 */
void SipRegister::registerCheckProc(void* param) {
    time_t regTime = 0;
    struct sysinfo info;
    memset(&info, 0, sizeof(info));
    int ret = sysinfo(&info);
    if (ret == 0) {
        regTime = info.uptime;
    } else {
        regTime = time(NULL);
    }
    AutoMutexLock lock(&(GlobalCtl::globalLock));
    GlobalCtl::SUBDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSubDomainInfoList().begin();
    for (;iter != GlobalCtl::instance()->getSubDomainInfoList().end(); iter++) {
        if (iter->registered) {
            LOG(INFO) << "regTime:" <<regTime<< ",lastRegTime:" <<iter->lastRegTime;
            if (regTime - iter->lastRegTime >= iter->expires) {
                iter->registered = false;
                LOG(INFO) << "register time was gone";
            }
        }
    }
}

/**
 *  实际处理业务逻辑的入口函数,继承自基类虚函数,可以处理不同的业务逻辑
 */
pj_status_t SipRegister::run(pjsip_rx_data* rdata) {
    return registerRequestMessage(rdata);
}

/**
 *  处理注册请求
 */
pj_status_t SipRegister::registerRequestMessage(pjsip_rx_data* rdata) {
    pjsip_msg* msg = rdata->msg_info.msg;
    // 依据是否开启鉴权,进入不同的业务处理函数
    if (GlobalCtl::getAuth(parseFromId(msg))) {
        return dealWithAuthorRegister(rdata);
    } else {
        return dealWithRegister(rdata);
    }
}

/**
 *  上级处理下级非鉴权注册
 */
pj_status_t SipRegister::dealWithRegister(pjsip_rx_data* rdata) {
    int status_code = 200;

    // 获取from字段
    string random = GlobalCtl::randomNum(32);
    LOG(INFO) << "random:" << random;

    // 截取from字段中的from Id,判断下级是否合法
    pjsip_msg* msg = rdata->msg_info.msg;
    string fromId = parseFromId(msg);
    LOG(INFO) << "fromId:" << fromId;

    // 对多个下级进行白名单查询,如果找到了from Id对应的下级,那么就解析Expires字段的值,如果是0,那就是下级向上级发送的注销请求,如果大于0,那么
    // 如果不是from Id对应的下级，那么上级就响应给下级状态码404
    pj_int32_t expiresValue = 0;
    if (!(GlobalCtl::checkIsExist(fromId))) {
        status_code = SIP_FORBIDDEN;
    } else {
        pjsip_expires_hdr* expires = (pjsip_expires_hdr*)pjsip_msg_find_hdr(msg, PJSIP_H_EXPIRES, NULL);
        expiresValue = expires->ivalue;
        GlobalCtl::setExpires(fromId,expiresValue);
    }

    //创建txdata数据结构
    pjsip_tx_data* txdata;
    pj_status_t status = pjsip_endpt_create_response(GBOJ(gSipServer)->GetEndPoint(), rdata, status_code, NULL, &txdata);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "create response failed";
        return status;
    }
    
    // 填充Data字段
    time_t t;
    t = time(0);
    char bufT[32] = {0};
    strftime(bufT,sizeof(bufT), "%y-%m-%d%H:%M:%S", localtime(&t));
    pj_str_t value_time = pj_str(bufT);
    pj_str_t key = pj_str("Date");
    pjsip_date_hdr* date_hrd = (pjsip_date_hdr*)pjsip_date_hdr_create(rdata->tp_info.pool, &key, &value_time);
    pjsip_msg_add_hdr(txdata->msg, (pjsip_hdr*)date_hrd);

    // 获取响应地址
    pjsip_response_addr res_addr;
     status = pjsip_get_response_addr(txdata->pool, rdata, &res_addr);
    if (PJ_SUCCESS != status) {
        pjsip_tx_data_dec_ref(txdata);
        LOG(ERROR) << "get response addr failed";
        return status;
    }
    
    // 发送响应
    status = pjsip_endpt_send_response(GBOJ(gSipServer)->GetEndPoint(), &res_addr, txdata, NULL, NULL);
    if (PJ_SUCCESS != status) {
        pjsip_tx_data_dec_ref(txdata);
        LOG(ERROR) << "send response msg failed";
        return status;
    }

    // 重新获取响应码,如果是20,那么注册成功,再判断Expires字段,是不是0,是0就是注销
    if (status_code == 200) {
        if (expiresValue > 0) {
            time_t regTime = 0;
            struct sysinfo info;
            memset(&info, 0, sizeof(info));
            int ret = sysinfo(&info);
            if (ret == 0) {
                regTime = info.uptime;
            } else {
                regTime = time(NULL);
            }
            GlobalCtl::setRegister(fromId, true);
            GlobalCtl::setLastRegTime(fromId, regTime);
        }
        else if(expiresValue == 0) {
            GlobalCtl::setRegister(fromId,false);
            GlobalCtl::setLastRegTime(fromId,0);
        }
    }
}

/**
 *  上级处理下级鉴权注册
 */
pj_status_t SipRegister::dealWithAuthorRegister(pjsip_rx_data *rdata) {
    pjsip_msg* msg = rdata->msg_info.msg;
    string fromId = parseFromId(msg);
    int expiresValue = 0;
    pjsip_hdr hdr_list;
    pj_list_init(&hdr_list);
    int status_code = 401;
    pj_status_t status;
    bool registered = false;

    // 配置文件中开启了鉴权,回复下级401,让下级进行鉴权注册
    if(pjsip_msg_find_hdr(msg, PJSIP_H_AUTHORIZATION, NULL) == NULL) {
        pjsip_www_authenticate_hdr* hdr = pjsip_www_authenticate_hdr_create(rdata->tp_info.pool);
        hdr->scheme = pj_str("digest");
        //nonce
        string nonce = GlobalCtl::randomNum(32);
        LOG(INFO)<<"nonce:"<<nonce;
        hdr->challenge.digest.nonce = pj_str((char*)nonce.c_str());
        //realm
        hdr->challenge.digest.realm = pj_str((char*)GBOJ(gConfig)->realm().c_str()); 
        //opaque
        string opaque = GlobalCtl::randomNum(32);
        LOG(INFO) << "opaque:" << opaque;
        hdr->challenge.digest.opaque = pj_str((char*)opaque.c_str());
        //加密方式
        hdr->challenge.digest.algorithm = pj_str("MD5");
        pj_list_push_back(&hdr_list, hdr);
    } else {
        // 用户名密码认证
        pjsip_auth_srv auth_srv;
        pj_str_t realm = pj_str((char*)GBOJ(gConfig)->realm().c_str());
        status = pjsip_auth_srv_init(rdata->tp_info.pool,&auth_srv,&realm,&auth_cred_callback,0);
        if (PJ_SUCCESS != status) {
            LOG(ERROR)<<"pjsip_auth_srv_init failed";
            status_code = 401;
        }
        // 本机身份信息与下级返回的身份信息进行对比
        pjsip_auth_srv_verify(&auth_srv, rdata, &status_code);
        LOG(INFO) << "status_code:" << status_code;
        if (SIP_SUCCESS == status_code) {
            pjsip_expires_hdr* expires = (pjsip_expires_hdr*)pjsip_msg_find_hdr(msg, PJSIP_H_EXPIRES, NULL);
            expiresValue = expires->ivalue;
            GlobalCtl::setExpires(fromId,expiresValue);

            //data字段hdr部分组织
            time_t t;
            t = time(0);
            char bufT[32] = {0};
            strftime(bufT,sizeof(bufT), "%y-%m-%d%H:%M:%S", localtime(&t));
            pj_str_t value_time = pj_str(bufT);
            pj_str_t key = pj_str("Date");
            pjsip_date_hdr* date_hrd = (pjsip_date_hdr*)pjsip_date_hdr_create(rdata->tp_info.pool, &key, &value_time);
            pj_list_push_back(&hdr_list, date_hrd); 
            registered = true;
        }
    }

    // 上级对下级进行响应
    status = pjsip_endpt_respond(GBOJ(gSipServer)->GetEndPoint(), NULL, rdata, status_code, NULL, &hdr_list, NULL, NULL);
    if (PJ_SUCCESS != status) {
        LOG(ERROR) << "pjsip_endpt_respond failed";
        return status;
    }

    // 对已经注册过的下级进行定时生存期检查
    if (registered) {
        if (expiresValue > 0) {
            time_t regTime = 0;
            struct sysinfo info;
            memset(&info,0,sizeof(info));
            int ret = sysinfo(&info);
            if (ret == 0) {
                regTime = info.uptime;
            } else {
                regTime = time(NULL);
            }
            GlobalCtl::setRegister(fromId, true);
            GlobalCtl::setLastRegTime(fromId, regTime);
        }
        else if (expiresValue == 0) {
            GlobalCtl::setRegister(fromId, false);
            GlobalCtl::setLastRegTime(fromId, 0);
        }
    }
}

