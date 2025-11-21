#ifndef SIP_LOCAL_CONFIG_H
#define SIP_LOCAL_CONFIG_H

#include "ConfReader.h"
#include "Common.h"
#include <list>
#include <algorithm>
#include <queue>


/**
 *  下级配置
 */ 
class SipLocalConfig {
private:
    ConfReader m_conf;
    string m_localIp;
    int m_localPort;
    string m_sipId;
    string m_sipIp;
    int m_sipPort;
    // string m_usr;
    // string m_pwd;
    // string m_sipRealm;
    string m_supNodeIp;
    int m_supNodePort;
    int m_supNodePoto;
    int m_supNodeAuth;
    int m_rtpPortBegin;
    int m_rtpPortEnd;
    std::queue<int> m_RandNum;
    pthread_mutex_t m_rtpPortLock;
public:
    struct SupNodeInfo {
        string id;
        string ip;
        int port;
        int poto;
        int auth;
        int expires;
        string usr;
        string pwd;
        string realm;
    };
    list<SupNodeInfo> supNodeInfoList;
public:
    SipLocalConfig();
    ~SipLocalConfig();

    int ReadConf();
    void initRandPort();
    int popOneRandNum();
    int pushOneRandNum(int num);
    
    inline string localIp(){return m_localIp;}
    inline int localPort(){return m_localPort;}
    inline int sipPort(){return m_sipPort;}
    inline string sipId(){return m_sipId;}
    inline string sipIp(){return m_sipIp;}
};
#endif
