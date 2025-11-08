#ifndef SIP_CORE_H
#define SIP_CORE_H

#include <pjlib>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip/sip_auth.h>

class SipCore {
private:
    /**
     * endpoint对象
     */
    pjsip_endpoint* m_endpt;
public:
    SipCore();
    ~SipCore();

    bool initSip(int sipPort);

    /**
     * 初始化transport层对象的接口
     */
    pj_status_t initTransportLayer(int sipPort);
};


#endif