#ifndef SIP_LOCAL_CONFIG_H
#define SIP_LOCAL_CONFIG_H

#include "config_reader.h"
#include "common.h"
#include <list>
#include <algorithm>
#include <queue>


/**
 * 上级服务器配置
 */
class SipLocalConfig {
public:
    SipLocalConfig();
    ~SipLocalConfig();
    int readConfig();
    
    inline std::string localIp() const { return m_localIp; }
    inline int localPort() const { return m_localPort; }
    inline std::string sipIp() const { return m_sipIp; }
    inline int sipPort() const { return m_sipPort; }
private:
    ConfigReader m_conf;

    std::string m_localIp;
    int m_localPort;
    std::string m_sipIp;
    int m_sipPort;

    std::string m_subNodeIp;
    int m_subNodePort;
    int m_subNodePoto;
    int m_subNodeAuth;
};


#endif