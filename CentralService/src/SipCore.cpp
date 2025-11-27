#include <tinyxml2.h>
#include <thread/ECThread.h>
#include <sip/SipDef.h>
#include <sip/SipTaskBase.h>
#include "SipCore.h"
#include "GlobalCtl.h"
#include "SipRegister.h"

using namespace EC;

SipCore::SipCore()
:m_endpt(NULL) {}

SipCore::~SipCore() {
    pjmedia_endpt_destroy(m_mediaEndpt);
    pjsip_endpt_destroy(m_endpt);
    pj_caching_pool_destroy(&m_cachingPool);
    pj_shutdown();
    GlobalCtl::gStopPoll = true;
}

/**
 *  SipCore的线程入口函数
 */
void* SipCore::dealTaskThread(void* arg) {
    threadParam* param = (threadParam*)arg;
    if (!param || param->base == NULL) {
        return NULL;
    }
    // 将此线程注册为pjsip管理的线程
    pj_thread_desc desc;
    pjcall_thread_register(desc);
    param->base->run(param->data);

    delete param;
    param = NULL;

    // 入口线程没有回返值,造成未定义行为,如果涉及到多个进行交互会直接触发Trace/breakpoint trap (core dumped)
    // 也就是下级一发来请求,上级进程显示:Trace/breakpoint trap (core dumped)
    return NULL;
}

/**
 *  利用多态在此回调函数中实现不同事件的业务逻辑
 */
pj_bool_t onRxRequest(pjsip_rx_data *rdata) {
    LOG(INFO) << "request msg coming ...";
    if (rdata == NULL || rdata->msg_info.msg == NULL) {
        return PJ_FALSE;
    }

    // 初始化
    threadParam* param = new threadParam();
    pjsip_rx_data_clone(rdata, 0, &param->data);

    // 获取SIP消息体
    pjsip_msg* msg = rdata->msg_info.msg;

    // 依据Reuqest-Line中的Method字段区分请求的业务
    LOG(INFO) << "request method id:" << msg->line.req.method.id;
    LOG(INFO) << "request method name:" << msg->line.req.method.name.ptr;

    if (msg->line.req.method.id == PJSIP_REGISTER_METHOD) {
        // 如果发送的请求的方法是REGISTER
        param->base = new SipRegister();
    } else if (msg->line.req.method.id == PJSIP_OTHER_METHOD) {
        // 如果发送的请求的方法是MESSAGE
        string rootType = "", cmdType = "CmdType", cmdValue;
        tinyxml2::XMLElement* root = SipTaskBase::parseXmlData(msg, rootType, cmdType, cmdValue);
        LOG(INFO) << "rootType:" << rootType;
        LOG(INFO) << "cmdValue:" << cmdValue;
        // if (rootType == SIP_NOTIFY && cmdValue == SIP_HEARTBEAT) {
        //     param->base = new SipHeartBeat();
        // }
        // else if (rootType == SIP_RESPONSE) {
        //     if (cmdValue == SIP_CATALOG) {
        //         param->base = new SipDirectory(root);
        //     }
        //     else if (cmdValue == SIP_RECORDINFO) {
        //         param->base = new SipRecordList(root);
        //     }
        // }
    }

    // 创建线程
    pthread_t pid;
    int ret = EC::ECThread::createThread(SipCore::dealTaskThread, param, pid);
    if (ret != 0) {
        LOG(ERROR) << "create task thread error";
        if (param) {
            delete param;
            param = NULL;
        }
        return PJ_FALSE;
    }
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

/**
 *  轮询线程入口回调函数
 */
static int pollingEvent(void* arg) {
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

        // 创建pjsip的endpoint对象
        status = pjsip_endpt_create(&m_cachingPool.factory, NULL, &m_endpt);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "create endpt faild,code:" << status;
            break;
        }
        // 创建pjsip的pjmedia对象
        pj_ioqueue_t* ioqueue = pjsip_endpt_get_ioqueue(m_endpt);
        status = pjmedia_endpt_create(&m_cachingPool.factory, ioqueue, 0, &m_mediaEndpt);
        if (PJ_SUCCESS != status) {
            LOG(ERROR)<<"create media endpoint faild,code:"<<status;
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

        // 将自定模块注册到endpoint中
        pjsip_endpt_register_module(m_endpt, &recv_mod);
        if (PJ_SUCCESS != status) {
            LOG(ERROR) << "register recv_mod faild,code:" << status;
            break;
        }
/*
        // 添加对INVITE事件的回调的初始化
        pjsip_inv_callback inv_cb;
        pj_bzero(&inv_cb,  sizeof(inv_cb));
        inv_cb.on_state_changed = NULL;
        inv_cb.on_new_session = NULL;
        inv_cb.on_media_update = NULL;
        inv_cb.on_send_ack = NULL;
        // 将INVITE事件的回调函数注册到endpoint对象中
        status = pjsip_inv_usage_init(m_endpt,&inv_cb);
        if (PJ_SUCCESS != status) {
            LOG(ERROR)<<"register invite module faild,code:"<<status;
            break;
        }
*/
        // 给endpoint分配内存池后,endpoint才能对其他模块进行内存分配管理
        // de用于处理网络媒体数据的收发，以及流的传输的功能
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