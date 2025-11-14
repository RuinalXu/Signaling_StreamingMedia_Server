#include "SipCore.h"
#include "common.h"
#include "sip_define.h"


SipCore::SipCore():
m_endpt(NULL) {

}

SipCore::~SipCore() {
    pjsip_endpt_destory(m_endpt);
    pj_caching_pool_destroy(&cachingPool);
    pj_shutdown();
}

bool SipCore::initSip(int sipPort) {
    pj_status_t status;
    // 0-关闭日志, 6-详细日志
    pj_log_set_level(6);

    // 调用pjsip接口进行相关库的初始化
    do {
        // 初始化pjlib库
        status = pj_init();
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "init pjlib faild, code:" << status;
            break;
        }
        
        // 初始化pjlib-utils库
        status = pjlib_util_init();
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "init pjlib util faild, code:" << status;
            break;
        }

        // 初始化pjsip内存池,pjsip的核心endpoint对象需要由内存池工厂进行分配内存创建
        pj_caching_pool cachingPool;
        pj_caching_pool_init(&cachingPool, NULL, SIP_STACK_SIZE);

        status = pjsip_endpt_create(&cachingPool.factoy, NULL, &m_endpt);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "create endpt faild, code:" << status;
            break;
        }

        // 初始化transcation对象
        status = pjsip_tsx_layer_init_module(m_endpt);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "init tsx layer faild, code:" << status;
            break;
        }

        // 初始化dialog对象
        status = pjsip_ua_init_module(e_endpt, NULL);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "init UA layer faild, code:" << status;
            break;
        }

        // 初始化transport对象
        status = SipCore::initTransportLayer(sipPort);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "init tansport layer faild, code:" << status;
            break;
        }

    } while (0);

    bool b_ret = true;
    if (PJ_SUCESS != status) {
        b_ret = false;
    }

    return b_ret;
}

/**
 * transport层就是负责将sip消息发送到网路并接收来在网络的sip消息；
 * pjsip的传输层是支持udp、tcp、tls
 */
pj_status_t SipCore::initTransportLayer(int sipPort) {
    // pjsip封装的表示IPV4的结构体
    pjsockaddr_in addr;
    pj_bzero(&addr, sizeof(addr));
    // 指定协议族
    addr.sin_family = pj_AF_INET();
    // 地址,此处设置为本机
    addr.sin_addr.s_addr = 0;
    addr.sin_port = pj_htons((pj_uint_16_t)sipPort);

    pj_status_t status;

    // 调用transport的接口来启动UDP和TCP服务
    do {
        status = pjsip_udp_transport_start(m_endpt, &daar, NULL, 1, NULL);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "start udp server faild, code:" << status;
            break;
        }

        status = pjsip_tcp_transport_start(m_endpt, &daar, NULL, 1, NULL);
        if (PJ_SUCESS != status) {
            LOG(ERROR) << "start tcp server faild, code:" << status;
            break;
        }

        LOG(INFO) << "sip tcp:" << sipPort << " udp:" << sipPort << " running";
    } while (0)

    return status;
}