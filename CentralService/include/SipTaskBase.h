#ifndef SIP_TASK_BASE_H
#define SIP_TASK_BASE_H
#include "Common.h"
/**
 *  上级用于处理不同类型SIP请求的基类
 */
class SipTaskBase {
public:
    SipTaskBase(){}
    virtual ~SipTaskBase() {
        LOG(INFO) << "~SipTaskBase";
    }
    virtual pj_status_t run(pjsip_rx_data* rdata) = 0;
};

#endif
