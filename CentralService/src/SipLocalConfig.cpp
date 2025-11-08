#include "SipLocalConfig.h"
#include <glog/logging.h>

#define CONFIG_FILE_PATH "config/SipSupService.conf"
#define LOCAL_SECTION "localserver"
#define SIP_SECTION "sipserver"

static const std::string keyLocalIp      = "local_ip";
static const std::string keyLocalPort    = "local_port";
static const std::string keySipIp        = "sip_ip";
static const std::string keySipPort      = "sip_port";

static const std::string keySubNodeNum   = "subnode_num";
static const std::string keySubNodeIp    = "sip_subnode_ip";
static const std::string keySubNodePort  = "sip_subnode_port";
static const std::string keySubNodePoto  = "sip_subnode_poto";

SipLocalConfig::SipLocalConfig()
: m_conf(CONFIG_FILE_PATH) {
    m_localIp = "";
    m_localPort = 0;
    m_sipIp = "";
    m_sipPort = 0;
    m_subNodeIp = "";
    m_subNodePort = 0;
    m_subNodePoto = 0;
    m_subNodeAuth = 0;
}

SipLocalConfig::~SipLocalConfig(){}

int SipLocalConfig::readConfig() {
    int ret = 0;
    m_conf.setSection(LOCAL_SECTION);
    m_localIp = m_conf.readStr(keyLocalIp);
    if (m_localIp.empty()) {
        ret = -1;
        LOG(ERROR) << "localIp is wrong";
        return ret;
    }
    m_localPort = m_conf.readInt(keyLocalPort);
    if (m_localPort <= 0) {
        ret = -1;
        LOG(ERROR) << "localport is wrong";
    }

    m_conf.setSection(SIP_SECTION);
    m_sipIp = m_conf.readStr(keySipIp);
    if (m_sipIp.empty()) {
        ret = -1;
        LOG(ERROR) << "m_sipIp is wrong";
        return ret;
    }
    m_sipPort = m_conf.readInt(keySipPort);
    if (m_sipPort <= 0) {
        ret = -1;
        LOG(ERROR) << "m_sipPort is wrong";
    }

    LOG(INFO) << "localip: " << m_localIp << ", " << "lcoalport: " << m_localPort << "\t"
        << "sipip: " << m_sipIp << ", " << "sipport: " << m_sipPort;

    
    return ret;
}
