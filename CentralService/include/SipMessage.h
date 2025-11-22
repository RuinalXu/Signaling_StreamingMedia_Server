#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H

#include "Common.h"
#include "GlobalCtl.h"

/**
 *  SIP消息封装类
 */
class SipMessage {
private:
    char fromHeader[128];
    char toHeader[128];
    char requestUrl[128];
    char contact[128];
public:
    SipMessage();
    ~SipMessage();

    void setFrom(char* fromUsr, char* fromIp);
    void setTo(char* toUsr, char* toIp);
    void setUrl(char* sipId, char* url_ip, int url_port, char* url_proto = (char*)"udp");
    void setContact(char* sipId, char* natIp, int natPort);

    inline char* FromHeader(){ return fromHeader; }
    inline char* ToHeader(){ return toHeader; }
    inline char* RequestUrl(){ return requestUrl; }
    inline char* Contact(){ return contact; }
};

#endif
