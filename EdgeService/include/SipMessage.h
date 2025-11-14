#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H


/**
 * sip消息包字段组织功能封装
 */
class SipMessage
{
private:
    char* fromHeader[128];
    char* toHeader[128];
    char* requestUrl[128];
    char* contact[128];
public:
    SipMessage(/* args */);
    ~SipMessage();

    void setFrom(char* fromUser, char* fromIp);
    void setTO(char* toUsr, char* toIp);
    void setUrl(char* sipId, char* url_ip, int url_port, char* url_proto = (char*)"udp");
    void setContact(char* sipId, char* natIp, int natPort);
};



#endif