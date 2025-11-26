#include "SipDirectory.h"
#include <fstream>
#include <sstream>
#include <sip/SipMessageHeader.h>
#include "GlobalCtl.h"

SipDirectory::SipDirectory(tinyxml2::XMLElement* root) 
:SipTaskBase() {
    m_pRootElement = root;
}

SipDirectory::~SipDirectory() {}

void SipDirectory::run(pjsip_rx_data *rdata)
{
	LOG(INFO) << "SipDirectory::run";
    int sn = -1;
    resDir(rdata,&sn);
    Json::Value jsonOut;
    directoryQuery(jsonOut);

    char* sendData = new char[BODY_SIZE];
    int sum = jsonOut["data"]["nodeInfo"].size();
    int begin = 0;
    int sendCnt = 1;
    while (begin < sum) {
        memset(sendData, 0, BODY_SIZE);
        constructMANSCDPXml(jsonOut["data"]["nodeInfo"], &begin, sendCnt, sendData, sn);
        sendSipDirMsg(rdata, sendData);
        usleep(15 * 1000);
    }
    return;
}

void SipDirectory::constructMANSCDPXml(Json::Value listdata, int* begin, int itemCount, char* sendData, int sn) {
    XmlParser parse;
    tinyxml2::XMLElement* rootNode = parse.addRootNode((char*)"Response");
    parse.insertSubNode(rootNode, (char*)"CmdType", (char*)"Catalog");
    char tmpStr[32] = {0};
    sprintf(tmpStr,"%d",sn);
    parse.InsertSubNode(rootNode,(char*)"SN",tmpStr);
    parse.InsertSubNode(rootNode,(char*)"DeviceID",GBOJ(gConfig)->sipId().c_str());
    memset(tmpStr,0,sizeof(tmpStr));
    sprintf(tmpStr,"%d",listdata.size());
    parse.InsertSubNode(rootNode,(char*)"SumNum",tmpStr);
    tinyxml2::XMLElement* itemNode = parse.InsertSubNode(rootNode,(char*)"DeviceList",(char*)"");
    memset(tmpStr,0,sizeof(tmpStr));
    sprintf(tmpStr,"%d",itemCount);
    parse.SetNodeAttributes(itemNode,(char*)"Num",tmpStr);
    int i = *begin;
    int index = 0;
    for(;i<listdata.size();i++)
    {
        if(index == itemCount)
        {
            break;
        }

        Json::Value &devNode = listdata[i];
        tinyxml2::XMLElement* node = parse.InsertSubNode(itemNode,(char*)"item",(char*)"");
        parse.InsertSubNode(node,(char*)"DeviceID",(char*)devNode["deviceID"].asString().c_str());
        parse.InsertSubNode(node,(char*)"Name",(char*)devNode["name"].asString().c_str());
        //当为设备时  为必选
        if(devNode["manufacturer"] == "")
        {
            devNode["manufacturer"] = "unknow";
        }
        parse.InsertSubNode(node,(char*)"Manufacturer",(char*)devNode["manufacturer"].asString().c_str());
        //为设备时  必选  设备型号
        if(devNode["model"] == "")
        {
            devNode["model"] = "unknow";
        }
        parse.InsertSubNode(node,(char*)"Model",(char*)devNode["model"].asString().c_str());

        parse.InsertSubNode(node,(char*)"Owner",(char*)"unknow");
        
        string civilCode = devNode["deviceID"].asString().substr(0,6);
        parse.InsertSubNode(node,(char*)"CivilCode",(char*)civilCode.c_str());

        char info[32] = {0};
        int parental = devNode["parental"].asInt();
        sprintf(info,"%d",parental);
        parse.InsertSubNode(node,(char*)"Parental",info);

        parse.InsertSubNode(node,(char*)"ParentID",(char*)devNode["parentID"].asString().c_str());
        
        int safeway = devNode["safetyWay"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",safeway);
        parse.InsertSubNode(node,(char*)"SafetyWay",info);

        int registerway = devNode["registerWay"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",registerway);
        parse.InsertSubNode(node,(char*)"RegisterWay",info);

        int secrecy = devNode["secrecy"].asInt();
        memset(info,0,32);
        sprintf(info,"%d",secrecy);
        parse.InsertSubNode(node,(char*)"Secrecy",info);

        parse.InsertSubNode(node,(char*)"Status",(char*)devNode["status"].asString().c_str());
        
        index++;
    }
    *begin = i;
    parse.getXmlData(sendData);
    return;
}

void SipDirectory::sendSipDirMsg(pjsip_rx_data *rdata,char* sendData)
{
    LOG(INFO)<<sendData;
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
    pjsip_method reqMethod = {PJSIP_OTHER_METHOD,{(char*)method.c_str(),method.length()}};
    pjsip_tx_data* tdata;
    pj_status_t status = pjsip_endpt_create_request(GBOJ(gSipServer)->GetEndPoint(),&reqMethod,&line,&from,&to,NULL,NULL,-1,NULL,&tdata);
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

void SipDirectory::resDir(pjsip_rx_data *rdata,int* sn)
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

        string devid,strSn;
        tinyxml2::XMLElement* pElement = pRootElement->FirstChildElement("DeviceID");
        if(pElement && pElement->GetText())
            devid = pElement->GetText();
        
        if(devid != GBOJ(gConfig)->sipId())
        {
            status_code = 400;
            break;
        }

        pElement = pRootElement->FirstChildElement("SN");
        if(pElement)
            strSn = pElement->GetText();
        *sn = atoi(strSn.c_str());

    }while(0);

    pj_status_t status = pjsip_endpt_respond(GBOJ(gSipServer)->GetEndPoint(),NULL,rdata,status_code,NULL,NULL,NULL,NULL);
    if(PJ_SUCCESS != status)
    {
        LOG(ERROR)<<"pjsip_endpt_respond error";
    }
    return;

}

void SipDirectory::directoryQuery(Json::Value& jsonOut)
{
    std::ifstream file("../../conf/catalog.json");
    std::stringstream buffer;
    buffer<<file.rdbuf();
    string payload = buffer.str();

    if(JsonParse(payload).toJson(jsonOut) == false)
    {
        LOG(ERROR)<<"JsonParse error";
    }
    return;
}
