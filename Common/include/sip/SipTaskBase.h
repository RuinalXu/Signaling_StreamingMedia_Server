#ifndef SIP_TASK_BASE_H
#define SIP_TASK_BASE_H

#include <pjlib.h>
#include <pjsip_ua.h>
#include "sip/SipTaskBase.h"
#include "tinyxml2.h"
#include "log/LogManager.h"

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
    std::string parseToId(pjsip_msg* msg);
};

#endif
