#include "SipCore.h"
#include "SipDef.h"
#include "GlobalCtl.h"

SipCore::SipCore()
:m_endpt(NULL) {

}

SipCore::~SipCore() {
    pjsip_endpt_destroy(m_endpt);
    pj_caching_pool_destroy(&m_cachingPool);
    pj_shutdown();
    GlobalCtl::gStopPoll = true;
}

/**
 *  轮询线程入口回调函数
 */
static int pollingEvent(void* arg)
{
    LOG(INFO) << "poolling event thread start success";
    pjsip_endpoint* ept = (pjsip_endpoint*)arg;
    while(!(GlobalCtl::gStopPoll)) {
        pj_time_val timeout = {0, 500};
        pj_status_t status = pjsip_endpt_handle_events(ept, &timeout);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "polling events failed,code:" << status;
            return -1;
        }
    }
    return 0;
}

pj_bool_t onRxRequest(pjsip_rx_data *rdata) {
    return PJ_SUCCESS;
}

static pjsip_module recv_mod =
{
    NULL,NULL,
    {"mod-recv",8},
    -1,
    PJSIP_MOD_PRIORITY_APPLICATION,
    NULL,
    NULL,
    NULL,
    NULL,
    onRxRequest,
    NULL,
    NULL,
    NULL,
    NULL,
};

bool SipCore::InitSip(int sipPort) {
    pj_status_t status;
    
    // 0-关闭; 6-最详细
    pj_log_set_level(0);

    do {
        // 初始化pjlib
        status = pj_init();
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "init pjlib faild,code:" << status;
            break;
        }

        // 初始化pjlib-util库
        status = pjlib_util_init();
        if(PJ_SUCCESS != status) {
            LOG(ERROR) << "init pjlib util faild,code:" << status;
            break;
        }

        // 初始化endpoint, 该对象需要有pjsip内存工厂分配创建
        // 初始化pjsip重要模块: endpoint, transcation, dialog, transport
        // 一个pjsip进程只有一个endpoint对象,其他三个模块都有由endpoint进行管理的.
        pj_caching_pool_init(&m_cachingPool, NULL, SIP_STACK_SIZE);
        status = pjsip_endpt_create(&m_cachingPool.factory, NULL, &m_endpt);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "create endpt faild,code:" << status;
            break;
        }

        // 初始化事务层
        status = pjsip_tsx_layer_init_module(m_endpt);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "init tsx layer faild,code:" << status;
            break;
        }

        // 初始化会话层
        status = pjsip_ua_init_module(m_endpt, NULL);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "init UA layer faild,code:" <<status;
            break;
        }

        // 初始化传输层,同时初始化TCP与UDP
        status = init_transport_layer(sipPort);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "init transport layer faild,code:" << status;
            break;
        }

        // 自定模块注册到endpoint中
        pjsip_endpt_register_module(m_endpt, &recv_mod);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "register recv_mod faild,code:" << status;
            break;
        }

        // 给endpoint分配内存池后,endpoint才能对其他模块进行内存分配管理
        m_pool = pjsip_endpt_create_pool(m_endpt, NULL, SIP_ALLOC_POOL_1M, SIP_ALLOC_POOL_1M);
        if (NULL == m_pool) {
            LOG(ERROR)<<"create pool faild";
            break;
        }
        // 创建线程轮询处理endpoint事务
        pj_thread_t* eventThread = NULL;
        status = pj_thread_create(m_pool, NULL, &pollingEvent, m_endpt, 0, 0, &eventThread);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "create pjsip thread faild,code:" << status;
            break;
        }
    } while(0);

    bool bret = true;
    if (PJ_SUCCESS != status) {
        bret = false;
    }
    return bret;
}

/**
 *  初始化传输层
 */
pj_status_t SipCore::init_transport_layer(int sipPort) {
    pj_sockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));
    addr.sin_family = pj_AF_INET();
    addr.sin_addr.s_addr = 0;
    addr.sin_port = pj_htons((pj_uint16_t)sipPort);
    pj_status_t status;
    do {
        status = pjsip_udp_transport_start(m_endpt, &addr, NULL, 1, NULL);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "start udp server faild,code:" << status;
            break;
        }
        status = pjsip_tcp_transport_start(m_endpt, &addr, 1, NULL);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "start tcp server faild,code:" << status;
            break;
        }
        LOG(INFO) << "sip tcp:" << sipPort << " udp:" << sipPort << " running";
    } while (0);
    return status;
}