#ifndef SIP_TASK_BASE
#define SIP_TASK_BASE

#include "common.h"

class SipTaskBase {
public:
    SipTaskBase(){};
    virtual ~SipTaskBase();

    /**
     * 处理不同注册业务
     */
    virtual pj_status_t run(pjsip_rx_data *rdata) = 0;

    /**
     * 从From中解析出Id字段,且只能基类的子类调用
     */
protected:
    std::string parseFromId(pjsip_msg* msg);
};

#endif
