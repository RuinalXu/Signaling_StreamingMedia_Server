#ifndef SIP_DIRECTORY_H
#define SIP_DIRECTORY_H

#include "SipTaskBase.h"
#include <utils/XmlParser.h>
#include <utils/JsonParser.h>

/**
 *  下级解析上级发送的XML获取到sn号,并响应目录树给上级.
 */
class SipDirectory : public SipTaskBase {
private:
    tinyxml2::XMLElement* m_pRootElement;
public:
    SipDirectory(tinyxml2::XMLElement* root);
    ~SipDirectory();

    /**
     *  下级处理上级的请求
     */
    virtual void run(pjsip_rx_data *rdata);
    void resDir(pjsip_rx_data *rdata, int* sn);
    void directoryQuery(Json::Value& jsonOut);
    void constructMANSCDPXml(Json::Value listdata, int* begin, int itemCount, char* sendData, int sn);
    void sendSipDirMsg(pjsip_rx_data *rdata, char* sendData);
};

#endif
