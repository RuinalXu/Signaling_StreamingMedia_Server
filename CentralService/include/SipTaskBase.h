#ifndef SIP_TASK_BASE_H
#define SIP_TASK_BASE_H

#include <log/LogManager.h>
#include <sip/SipInclude.h>
#include <tinyxml2.h>

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
    static tinyxml2::XMLElement* parseXmlData(pjsip_msg* msg, std::string& rootType, const std::string xmlkey, std::string& xmlvalue);
protected:
    std::string parseFromId(pjsip_msg* msg);
};

#endif
