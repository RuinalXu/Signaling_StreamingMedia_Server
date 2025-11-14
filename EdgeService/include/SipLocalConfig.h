#ifndef SIP_LOCAL_CONFIG_H
#define SIP_LOCAL_CONFIG_H

#include "config_reader.h"
#include "common.h"
#include <list>
#include <algorithm>
#include <queue>


/**
 * 下级服务器配置,在下级服务器配置文件中保存自身下级配置信息、上级服务器信息
 */
class SipLocalConfig {
public:
    SipLocalConfig();
    ~SipLocalConfig();
    int readConfig();
    
    inline std::string localIp() const { return m_localIp; }
    inline int localPort() const { return m_localPort; }
    inline std::string sipId() const { return m_sipId; }
    inline std::string sipIp() const { return m_sipIp; }
    inline int sipPort() const { return m_sipPort; }
private:
    ConfigReader m_conf;

    std::string m_localIp;
    int m_localPort;
    std::string m_sipIp;
    int m_sipPort;
    std::string m_sipId;

    std::string m_supNodeIp;
    int m_supNodePort;
    int m_supNodePoto;
    int m_supNodeAuth;
};


#endif