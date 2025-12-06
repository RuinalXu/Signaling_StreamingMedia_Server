#include "SipDirectory.h"
#include <fstream>
#include <sstream>
#include <sip/SipMessageHeader.h>
#include <sip/SipDef.h>
#include "GlobalCtl.h"

SipDirectory::SipDirectory(tinyxml2::XMLElement* root) 
:SipTaskBase() {
    m_pRootElement = root;
}

SipDirectory::~SipDirectory() {}

/**
 *  下级处理上级请求的接口
 */
void SipDirectory::run(pjsip_rx_data *rdata)
{
    // 解析上级发送的rdata,若没有问题先响应上级,再推送目录树

    // 目录树
    // 下级服务器为上级推送目录资源,数据的来源为数据库,数据库的数据为下级的客户端,
    // 那么这个连接下级服务的客户端就是提供数据库的CRUD业务,所以需要新增一个模块,实现数据库相关的业务
    // 并提供一个接口
    // 如何实现：使用libcul中的http/https去请求golang的接口，返回数据库中共享目录表中的数据
    // 以上为实际开发过程,但是在本项目中,我们是直接读取json文件
    // 把这个读取到的json数据作为查询到的目录树数据

    // 这里涉及到一个概念：网络数据传输/数据透传尽量选择json数据

    // 为了防止目录树解析失败,尽量将目录树数据XML中的Item字段一次完成发送,不要对Item字段进行拆分
    // 这是在实际过程中得到的经验


	LOG(INFO) << "SipDirectory::run";
    int sn = -1;
    resDir(rdata, &sn);
    Json::Value jsonOut;
    directoryQuery(jsonOut);

    char* sendData = new char[BODY_SIZE];
    int sum = jsonOut["data"]["nodeInfo"].size();
    int begin = 0;
    int sendCnt = 1;
    while (begin < sum) {
        memset(sendData, 0, BODY_SIZE);
        // 组织MESSAGE请求中body部分
        constructMANSCDPXml(jsonOut["data"]["nodeInfo"], &begin, sendCnt, sendData, sn);
        // 组织实际传输的目录信息
        sendSipDirMsg(rdata, sendData);
        usleep(15 * 1000);
    }
    return;
}

/**
 *  推送目录树.
 */
void SipDirectory::resDir(pjsip_rx_data *rdata, int* sn)
{
    int status_code = 200;

    do
    {
        tinyxml2::XMLElement* pRootElement = m_pRootElement;
        if(!pRootElement)
        {
            status_code = 400;
            break;
        }

        string devid, strSn;
        tinyxml2::XMLElement* pElement = pRootElement->FirstChildElement("DeviceID");
        if(pElement && pElement->GetText()){
            devid = pElement->GetText();
        }
        
        if(devid != GBOJ(gConfig)->sipId())
        {
            status_code = 400;
            break;
        }

        pElement = pRootElement->FirstChildElement("SN");
        if(pElement) {
            strSn = pElement->GetText();
        }
        *sn = atoi(strSn.c_str());

    }while(0);

    // 响应上级
    pj_status_t status = pjsip_endpt_respond(GBOJ(gSipServer)->GetEndPoint(), NULL, rdata, status_code, NULL, NULL, NULL, NULL);
    if(PJ_SUCCESS != status)
    {
        LOG(ERROR)<<"pjsip_endpt_respond error";
    }
    return;

}

/**
 *  模拟下级服务的客户端的目录树查询
 *  这里使用本地文件替代,实际开发过程中,是通过数据库查询到的
 */
void SipDirectory::directoryQuery(Json::Value& jsonOut) {
    std::ifstream file("../../conf/catalog.json");
    std::stringstream buffer;
    buffer << file.rdbuf();
    string payload = buffer.str();
    if (JsonParser(payload).toJson(jsonOut) == false) {
        LOG(ERROR) << "JsonParse error";
    }
    return;
}

/**
 *  组织Message Body中的XML目录树
 */
void SipDirectory::constructMANSCDPXml(Json::Value listdata, int* begin, int itemCount, char* sendData, int sn) {
    XmlParser parse;
    tinyxml2::XMLElement* rootNode = parse.addRootNode((char*)"Response");
    parse.insertSubNode(rootNode, (char*)"CmdType", (char*)"Catalog");
    char tmpStr[32] = {0};
    sprintf(tmpStr,"%d",sn);
    parse.insertSubNode(rootNode,(char*)"SN",tmpStr);
    parse.insertSubNode(rootNode,(char*)"DeviceID",GBOJ(gConfig)->sipId().c_str());
    memset(tmpStr,0,sizeof(tmpStr));
    sprintf(tmpStr,"%d",listdata.size());
    parse.insertSubNode(rootNode,(char*)"SumNum",tmpStr);
    tinyxml2::XMLElement* itemNode = parse.insertSubNode(rootNode,(char*)"DeviceList",(char*)"");
    memset(tmpStr,0,sizeof(tmpStr));
    sprintf(tmpStr,"%d",itemCount);
    parse.setNodeAttributes(itemNode,(char*)"Num",tmpStr);
    int i = *begin;
    int index = 0;
    for(;i<listdata.size();i++)
    {
        if(index == itemCount)
        {
            break;
        }

        // 组织目录树XML文件中的Item部分
        Json::Value &devNode = listdata[i];
        tinyxml2::XMLElement* node = parse.insertSubNode(itemNode,(char*)"item",(char*)"");
        parse.insertSubNode(node,(char*)"DeviceID",(char*)devNode["deviceID"].asString().c_str());
        parse.insertSubNode(node,(char*)"Name",(char*)devNode["name"].asString().c_str());
        // 当为设备时  为必选
        if(devNode["manufacturer"] == "")
        {
            devNode["manufacturer"] = "unknow";
        }
        parse.insertSubNode(node,(char*)"Manufacturer",(char*)devNode["manufacturer"].asString().c_str());
        // 为设备时  必选  设备型号
        if(devNode["model"] == "")
        {
            devNode["model"] = "unknow";
        }
        parse.insertSubNode(node,(char*)"Model",(char*)devNode["model"].asString().c_str());

        parse.insertSubNode(node,(char*)"Owner",(char*)"unknow");
        
        string civilCode = devNode["deviceID"].asString().substr(0,6);
        parse.insertSubNode(node,(char*)"CivilCode",(char*)civilCode.c_str());

        char info[32] = {0};
        int parental = devNode["parental"].asInt();
        sprintf(info,"%d",parental);
        parse.insertSubNode(node,(char*)"Parental",info);

        parse.insertSubNode(node,(char*)"ParentID",(char*)devNode["parentID"].asString().c_str());
        
        int safeway = devNode["safetyWay"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",safeway);
        parse.insertSubNode(node,(char*)"SafetyWay",info);

        int registerway = devNode["registerWay"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",registerway);
        parse.insertSubNode(node,(char*)"RegisterWay",info);

        int secrecy = devNode["secrecy"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",secrecy);
        parse.insertSubNode(node,(char*)"Secrecy",info);

        parse.insertSubNode(node,(char*)"Status",(char*)devNode["status"].asString().c_str());
        
        index++;
    }
    *begin = i;
    parse.getXmlData(sendData);
    return;
}

/**
 *  组织SIP的Message部分,包括Request Line/Message Header/Message Body部分.
 */
void SipDirectory::sendSipDirMsg(pjsip_rx_data *rdata,char* sendData) {
    LOG(INFO) << sendData;
    pjsip_msg* msg = rdata->msg_info.msg;
    string fromId = parseFromId(msg);
    SipMessage sipMsg;
    AutoMutexLock lock(&(GlobalCtl::globalLock));
    GlobalCtl::SUPDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSupDomainInfoList().begin();
    for(;iter != GlobalCtl::instance()->getSupDomainInfoList().end(); iter++)
    {
        if(iter->sipId == fromId)
        {
            sipMsg.setFrom((char*)GBOJ(gConfig)->sipId().c_str(),(char*)GBOJ(gConfig)->sipIp().c_str());
            sipMsg.setTo((char*)iter->sipId.c_str(),(char*)iter->addrIp.c_str());
            sipMsg.setUrl((char*)iter->sipId.c_str(),(char*)iter->addrIp.c_str(),iter->sipPort);
        }
        else
        {
            return;
        }
    }

    pj_str_t from = pj_str(sipMsg.FromHeader());
    pj_str_t to = pj_str(sipMsg.ToHeader());
    pj_str_t line = pj_str(sipMsg.RequestUrl());
    string method = "MESSAGE";
    pjsip_method reqMethod = {PJSIP_OTHER_METHOD, {(char*)method.c_str(), method.length()}};
    pjsip_tx_data* tdata;
    pj_status_t status = pjsip_endpt_create_request(GBOJ(gSipServer)->GetEndPoint(), &reqMethod, &line, &from, &to,NULL,NULL,-1,NULL,&tdata);
    if(PJ_SUCCESS != status)
    {
        LOG(ERROR)<<"pjsip_endpt_create_request ERROR";
        return ;
    }

    pj_str_t type = pj_str("Application");
    pj_str_t subtype = pj_str("MANSCDP+xml");
    pj_str_t xmldata = pj_str(sendData);
    tdata->msg->body = pjsip_msg_body_create(tdata->pool,&type,&subtype,&xmldata);

    status = pjsip_endpt_send_request(GBOJ(gSipServer)->GetEndPoint(),tdata,-1,NULL,NULL);
    if(PJ_SUCCESS != status)
    {
        LOG(ERROR)<<"pjsip_endpt_send_request ERROR";
        return;
    }
    return;
}