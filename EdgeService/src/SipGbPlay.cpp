#include "SipGbPlay.h"
#include <sip/SipInclude.h>
#include <log/LogManager.h>
#include "GlobalCtl.h"
#include <sip/SipDef.h>
#include <sip/SipMessageHeader.h>

SipGbPlay::SipGbPlay() {}

SipGbPlay::~SipGbPlay() {}

/**
 *  下级对上级的INVITE请求的分发
 */
void SipGbPlay::run(pjsip_rx_data *rdata) {
    pjsip_msg* msg = rdata->msg_info.msg;
    if(msg->line.req.method.id == PJSIP_INVITE_METHOD) {
        dealWithInvite(rdata);
    }
    else if(msg->line.req.method.id == PJSIP_BYE_METHOD) {
        // 实现下级处理上级的BYE请求
        dealWithBye(rdata);
    }
}

void SipGbPlay::dealWithInvite(pjsip_rx_data *rdata) {
    string fromId = parseFromId(rdata->msg_info.msg);
    bool flag = false;
    int status_code = 200;
    string id;
    MediaInfo sdpInfo;

    /* 解析上级发送的SDP信息 */
    do {
       {
            AutoMutexLock lock(&(GlobalCtl::globalLock));
            GlobalCtl::SUPDOMAININFOLIST::iterator iter = GlobalCtl::instance()->getSupDomainInfoList().begin();
            for(;iter != GlobalCtl::instance()->getSupDomainInfoList().end(); iter++) {
                if(iter->sipId == fromId && iter->registered) {
                    flag = true;
                    break;
                }
            }
        }

        if(!flag) {
            status_code = SIP_FORBIDDEN;
            break;
        }

        pjmedia_sdp_session* sdp = NULL;
        if(rdata->msg_info.msg->body) {
            // 获取rdata中的SDP信息
            pjsip_rdata_sdp_info* sdp_info = pjsip_rdata_get_sdp_info(rdata);
            sdp = sdp_info->sdp;
        }
        // SDP请求格式非法
        if(sdp && sdp->media_count == 0) {
            status_code = SIP_BADREQUEST;
            break;
        }

        // 判断开流设备
        string devId(sdp->origin.user.ptr, sdp->origin.user.slen);
        DevTypeCode type = GlobalCtl::getSipDevInfo(devId);
        if(type == Error_code) {
            status_code = SIP_FORBIDDEN;
            break;
        }
        id = devId;
        
        // 判断是实时流还是
        string tmp(sdp->name.ptr, sdp->name.slen);
        sdpInfo.sessionName = tmp;
        if(sdpInfo.sessionName == "PlayBack") {
            sdpInfo.startTime = sdp->time.start;
            sdpInfo.endTime = sdp->time.stop;
        }

        pjmedia_sdp_conn* c = sdp->conn;
        string dst_ip(c->addr.ptr, c->addr.slen);
        sdpInfo.dstRtpAddr = dst_ip;

        // 获取收流设备的端口
        pjmedia_sdp_media* m = sdp->media[sdp->media_count-1];
        int sdp_port = m->desc.port;
        sdpInfo.dstRtpPort = sdp_port;

        // 指定传输层的协议是TCP,还是UDP
        string protol(m->desc.transport.ptr, m->desc.transport.slen);
        sdpInfo.sdp_protol = protol;
        int poto = 0;
        if(sdpInfo.sdp_protol == "TCP/RTP/AVP") {
            poto = 1;
            pjmedia_sdp_attr* attr = pjmedia_sdp_attr_find2(m->attr_count, m->attr, "setup", NULL);
            string setup(attr->value.ptr, attr->value.slen);
            sdpInfo.setUp = setup;
        }

		// sdpInfo.localRtpPort = GBOJ(gConfig)->popOneRandNum();
        // ps = new SipPsCode(dst_ip,sdp_port,sdpInfo.localRtpPort,poto,sdpInfo.setUp,sdpInfo.startTime,sdpInfo.endTime);
        // {
        //     //需要在ps对象实例化后就插入到map中
        //     AutoMutexLock lck(&streamLock);
		// 	mediaInfoMap.insert(pair<string, SipPsCode*>(devId, ps));
        // }
    } while (0);

    /* 解析SDP信息完成,回复SIP MESSAGE给上级 */
    resWithSdp(rdata, status_code, id, sdpInfo);
}

void SipGbPlay::dealWithBye(pjsip_rx_data *rdata) {

}

/**
 *  下级回复上级,携带SDP信息
 */
void SipGbPlay::resWithSdp(pjsip_rx_data *rdata,int status_code,string devid, MediaInfo sdpInfo) {
    pjsip_tx_data* tdata;
    pjsip_endpt_create_response(GBOJ(gSipServer)->GetEndPoint(), rdata, status_code, NULL, &tdata);
    pj_str_t type = {"Application", 11};
    pj_str_t sdptype = {"SDP", 3};
    if(status_code != SIP_SUCCESS) {
        tdata->msg->body = pjsip_msg_body_create(tdata->pool, &type, &sdptype, &(pjsip_rdata_get_sdp_info(rdata)->body));
    } else {
        // 字符串流的方式组织SDP响应体信息
        stringstream ss;
        ss<<"v="<<"0"<<"\r\n";
        ss<<"o="<<devid<<" 0 0 IN IP4 "<<GBOJ(gConfig)->sipIp()<<"\r\n";
        ss<<"s="<<"Play"<<"\r\n";
        ss<<"c="<<"IN IP4 "<<GBOJ(gConfig)->sipIp()<<"\r\n";
        ss<<"t="<<"0 0"<<"\r\n";
        ss<<"m=video "<<30000<<" "<<sdpInfo.sdp_protol<<" 96"<<"\r\n";
        ss<<"a=rtpmap:96 PS/90000"<<"\r\n";
        ss<<"a=sendonly"<<"\r\n";
        if(sdpInfo.setUp != "") {
            if(sdpInfo.setUp == "passive") {
                ss<<"a=setup:"<<"active"<<"\r\n";
            }
            else if(sdpInfo.setUp == "active") {
                ss<<"a=setup:"<<"passive"<<"\r\n";
            }
        }
        string sResp = ss.str();
        pj_str_t sdpData = pj_str((char*)sResp.c_str());
        tdata->msg->body = pjsip_msg_body_create(tdata->pool, &type, &sdptype, &sdpData);
    }

    /* 发送SIP消息 */
    SipMessage msg;
	msg.setContact((char*)GBOJ(gConfig)->sipId().c_str(), (char*)GBOJ(gConfig)->sipIp().c_str(), GBOJ(gConfig)->sipPort());
	const pj_str_t contactHeader = pj_str("Contact");
	const pj_str_t param = pj_str(msg.Contact());
	pjsip_generic_string_hdr* customHeader = pjsip_generic_string_hdr_create(tdata->pool, &contactHeader, &param);
	pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)customHeader);
	pjsip_response_addr res_addr;
	pj_status_t status = pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
    if (PJ_SUCCESS != status) {
        pjsip_tx_data_dec_ref(tdata);
        return;
    }
    status = pjsip_endpt_send_response(GBOJ(gSipServer)->GetEndPoint(), &res_addr, tdata, NULL, NULL);
    if (PJ_SUCCESS != status) {
        pjsip_tx_data_dec_ref(tdata);
        return;
    }
    return;
}

/**
 *  将下级流媒体服务器直接作为
 *  流媒体服务器当作直接对接摄像头的服务器，该服务器可以直接从设备直接采集视频数据或者是音频数据，
 *  而且可以为每个通道的音频数据和视频数据进行自定义头部的标记，标记的业务可以根据具体的业务来定。
 */
/*int SipGbPlay::recvFrame() {
    // config目录中的stream.file文件当作下级从流媒体服务器拉来的流，那么就需要对该文件的每一帧进行header的解析，再解析出裸数据
    
    // 实现音视屏PS封装与发送

    // 视频关键帧: ps-header|ps system header|ps system map|pes header|h264 raw data
    // 视频非关键帧: ps-header|pes header|h264 raw data
    // 安防领域无B帧

    // 音频: pes header|aac raw data

    // 将封装的好的PS流交给jrtplib库进行RTP的封装与推送
    // sendPsRtpStream()

    // RTP负载的数据定位13000字节左右，后期还需要加上RTP包头，传输层协议包头，网络层IP协议包头，以及链路层以太网包头，所以在RTP端进行推送的时候，
    // 整个PS包算法的拆解，
    // 而且拆解发送的大小要小于网络层传输的mtu的最大值
    // 对视频和音频数据需要实现整包发送和分批发送算法


    // 下级推流，上级收流出现丢包的情况怎么解决的？
    

}*/

void SipGbPlay::onStateChanged(pjsip_inv_session *inv, pjsip_event *e) {
    LOG(INFO) << "SipGbPlay::onStateChanged";
}

void SipGbPlay::onNewSession(pjsip_inv_session *inv, pjsip_event *e) {
    LOG(INFO) << "SipGbPlay::onNewSession";
}

void SipGbPlay::onMediaUpdate(pjsip_inv_session *inv_ses, pj_status_t status) {
    LOG(INFO) << "SipGbPlay::onMediaUpdate";
}