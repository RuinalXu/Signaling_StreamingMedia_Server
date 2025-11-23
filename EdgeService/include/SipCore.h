#ifndef SIP_CORE_H
#define SIP_CORE_H

#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip/sip_auth.h>
#include <pjlib.h>
#include <pjsip_ua.h>


class SipCore {
private:
    pjsip_endpoint* m_endpt;
    pj_caching_pool m_cachingPool;
    pj_pool_t* m_pool;
public:
    SipCore();
    ~SipCore();

    bool InitSip(int sipPort);
    pj_status_t init_transport_layer(int sipPort);
    pjsip_endpoint* GetEndPoint() { return m_endpt; }
};


#endif
