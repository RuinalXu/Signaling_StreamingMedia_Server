#include "SipHeartBeat.h"
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sip/SipInclude.h>
#include <mutex/AutoMutexLock.h>
#include "GlobalCtl.h"

SipHeartBeat::SipHeartBeat() {}

SipHeartBeat::~SipHeartBeat() {}

pj_status_t SipHeartBeat::run(pjsip_rx_data* rdata) {
    return HeartBeatMessage(rdata);
}

/**
 *  上级处理下级发送过来的心跳包的函数
 */
pj_status_t SipHeartBeat::HeartBeatMessage(pjsip_rx_data *rdata) {
    LOG(INFO) << "HeartBeatMessage";
    // 获取rdata中message消息体
    pjsip_msg* msg = rdata->msg_info.msg;
    std::string fromId = parseFromId(msg);

    // 获取系统时间
    time_t regTime = 0;
    struct sysinfo info;
    memset(&info,0,sizeof(info));
    int ret = sysinfo(&info);
    if (ret == 0) {
        regTime = info.uptime;
    } else {
        regTime = time(NULL);
    }
    int status_code = 200;
    AutoMutexLock lck(&GlobalCtl::globalLock);
    GlobalCtl::SUBDOMAININFOLIST::iterator it;
    it = std::find(GlobalCtl::instance()->getSubDomainInfoList().begin(), GlobalCtl::instance()->getSubDomainInfoList().end(), fromId);
    if ((it != GlobalCtl::instance()->getSubDomainInfoList().end()) && it->registered) {
        it->lastRegTime = regTime;
    } else {
        status_code = 403;
    }

    // 响应消息给下级
    pj_status_t status = pjsip_endpt_respond(GBOJ(gSipServer)->GetEndPoint(), NULL, rdata, status_code, NULL, NULL, NULL, NULL);
    if (PJ_SUCCESS != status) {
        LOG(ERROR)<<"pjsip_endpt_respond error";
    }

    return 0;
}