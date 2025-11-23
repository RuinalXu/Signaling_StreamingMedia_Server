#include "SipHeartBeat.h"

SipHeartBeat::SipHeartBeat() {

}

SipHeartBeat::~SipHeartBeat() {

}

pj_status_t SipHeartBeat::run(pjsip_rx_data* rdata) {
    HeartBeatMessage(rdata);
}

/**
 *  上级处理下级发送过来的心跳包的函数
 */
pj_status_t SipHeartBeat::HeartBeatMessage(pjsip_rx_data *rdata) {
    // 获取rdata中message消息体

    // 获取系统时间

    // 判断是否成立

    // 响应消息给下级

    return 0;
}