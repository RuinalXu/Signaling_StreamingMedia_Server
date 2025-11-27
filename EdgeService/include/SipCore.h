#ifndef SIP_CORE_H
#define SIP_CORE_H

#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip/sip_auth.h>
#include <pjlib.h>
#include <pjsip_ua.h>
#include "SipTaskBase.h"

typedef struct _threadParam {
    _threadParam() {
        base = NULL;
        data = NULL;
    }
    ~_threadParam() {
        if (base) {
            delete base;
            base = NULL;
        }
        if (data) {
            pjsip_rx_data_free_cloned(data);
            data = NULL;
        }
    }

    SipTaskBase* base;
    pjsip_rx_data* data;
} threadParam;

class SipCore {
private:
    pjsip_endpoint* m_endpt;
    pj_caching_pool m_cachingPool;
    pj_pool_t* m_pool;
    pjmedia_endpt* m_mediaEndpt;
public:
    SipCore();
    ~SipCore();

    bool InitSip(int sipPort);
    pj_status_t init_transport_layer(int sipPort);
    pjsip_endpoint* GetEndPoint() { return m_endpt; }

    static void* dealTaskThread(void* arg);
};


#endif
