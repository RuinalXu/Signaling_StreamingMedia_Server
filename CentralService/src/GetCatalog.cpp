#include "GetCatalog.h"
#include <sip/SipMessageHeader.h>
#include <utils/XmlParser.h>
#include <mutex/AutoMutexLock.h>
#include "GlobalCtl.h"

GetCatalog::GetCatalog() {
    directoryGetPro(NULL);
}

GetCatalog::~GetCatalog() {}

/**
 *  给每个在线设备发送目录树请求的方法
 */
void GetCatalog::directoryGetPro(void* param) {
    SipMessage msg;
    XmlParser parse;
    AutoMutexLock lock(&(GlobalCtl::globalLock));
    GlobalCtl::SUBDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSubDomainInfoList().begin();
    // 轮询每一个在线节点
    for (; iter != GlobalCtl::instance()->getSubDomainInfoList().end(); iter++) {
        if (iter->registered) {
            msg.setFrom((char*)GBOJ(gConfig)->sipId().c_str(), (char*)GBOJ(gConfig)->sipIp().c_str());
            msg.setTo((char*)iter->sipId.c_str(), (char*)iter->addrIp.c_str());
            msg.setUrl((char*)iter->sipId.c_str(), (char*)iter->addrIp.c_str(),iter->sipPort);

            pj_str_t from = pj_str(msg.FromHeader());
            pj_str_t to = pj_str(msg.ToHeader());
            pj_str_t requestUrl = pj_str(msg.RequestUrl());

            // 创建SIP请求消息,包括Request Line和Message Header.
            string method = "MESSAGE";
            pjsip_method reqMethod = {PJSIP_OTHER_METHOD, {(char*)method.c_str(),method.length()}};
            pjsip_tx_data* tdata;
            pj_status_t status = pjsip_endpt_create_request(GBOJ(gSipServer)->GetEndPoint(), &reqMethod, &requestUrl, &from, &to, NULL, NULL, -1, NULL, &tdata);
            if (PJ_SUCCESS != status) {
                LOG(ERROR)<<"pjsip_endpt_create_request ERROR";
                return;
            }

            // 组织Message Body部分.
            tinyxml2::XMLElement* rootNode = parse.addRootNode((char*)"Query");
            parse.insertSubNode(rootNode, (char*)"CmdType", (char*)"Catalog");
            int sn = random() % 1024;
            char tmpStr[32] = {0};
            sprintf(tmpStr, "%d", sn);
            parse.insertSubNode(rootNode, (char*)"SN", tmpStr);
            parse.insertSubNode(rootNode, (char*)"DeviceID", iter->sipId.c_str());
            char* xmlbuf = new char[1024];
            memset(xmlbuf, 0, 1024);
            parse.getXmlData(xmlbuf);
            LOG(INFO) << "xml:" << xmlbuf;

            pj_str_t type = pj_str("Application");
            pj_str_t subtype = pj_str("MANSCDP+xml");
            pj_str_t xmldata = pj_str(xmlbuf);
            tdata->msg->body = pjsip_msg_body_create(tdata->pool, &type, &subtype, &xmldata);

            // 发送SIP请求
            status = pjsip_endpt_send_request(GBOJ(gSipServer)->GetEndPoint(), tdata, -1, NULL, NULL);
            if (PJ_SUCCESS != status) {
                LOG(ERROR)<<"pjsip_endpt_send_request ERROR";
                return;
            }
        }
    }
    return;
}
