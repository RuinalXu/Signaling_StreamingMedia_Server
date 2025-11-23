#include "SipRegister.h"
#include <time.h>

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

/**
 *  实际处理业务逻辑的函数
 */
pj_status_t SipRegister::run(pjsip_rx_data* rdata) {
    // 处理多种业务请求
    pjsip_msg* msg = (pjsip_msg*)rdata->msg_info.msg;
    return RegisterRequestMessage(msg);
}

/**
 *  处理注册请求
 */
pj_status_t SipRegister::RegisterRequestMessage(pjsip_msg* msg) {
    pjsip_from_hdr* from = (pjsip_from_hdr*)pjsip_msg_find_hdr(msg, PJSIP_H_FROM, NULL);
    // 依据是否开启鉴权,进入不同的业务处理函数
    // if () {
    //     // 鉴权注册
    //     dealWithAuthorRegister();
    // } else {
    //     dealWithRegister();
    // }
}

/**
 *  上级处理下级非鉴权注册
 */
pj_status_t SipRegister::dealWithRegister(pjsip_rx_data* rdata) {
    // 获取from字段
    string random = GlobalCtl::randomNum(32);
    LOG(INFO) << "random:" << random;
    pjsip_msg* msg = rdata->msg_info.msg;
    string fromId = parseFromId(msg);
    // 截取from字段中的from Id,判断下级是否合法

    // 对多个下级进行白名单查询,如果找到了from Id对应的下级,那么就解析Expires字段的值,如果是0,那就是下级向上级发送的注销请求,如果大于0,那么
    // 如果不是from Id对应的下级，那么上级就响应给下级状态码404


    // 开始响应
    
        // 获取endpoint对象,并创建响应体
        pjsip_tx_data* txdata;
        // 填充Data字段

        // 获取响应地址

        // 发送响应

    // 重新获取响应码,如果是20,那么注册成功,再判断Expires字段,是不是0,是0就是注销
    

}

/**
 *  上级处理下级鉴权注册
 */
pj_status_t SipRegister::dealWithAuthorRegister(pjsip_rx_data *rdata) {
    // 配置文件中开启了鉴权,回复下级401,让下级进行鉴权注册

    pjsip_endpt_create_response()

    pjsip_endpt_respond()
}

/**
 *  上级定时检查已注册下级的生存期
 */
void SipRegister::RegisterCheckProc(void* param) {

}

