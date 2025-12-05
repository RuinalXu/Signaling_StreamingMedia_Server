#include "Gb28181Session.h"
#include <log/LogManager.h>

void Gb28181Session::OnNewSource(RTPSourceData* srcdat) {
    LOG(INFO) << "Gb28181Session::OnNewSource";
    LOG(INFO) << "srcdat->IsOwnSSRC():" << srcdat->IsOwnSSRC();
    if (srcdat->IsOwnSSRC()) {
        return;
    }
    uint32_t ip;
    uint16_t port;
    LOG(INFO)<<"00";
    if (srcdat->GetRTPDataAddress() != 0) {
        LOG(INFO)<<"11";
        const RTPIPv4Address* addr = (const RTPIPv4Address*)srcdat->GetRTPDataAddress();
        ip = addr->GetIP();
        port = addr->GetPort();
    }
    else if (srcdat->GetRTCPDataAddress() != 0) {
        LOG(INFO)<<"22";
        const RTPIPv4Address* addr = (const RTPIPv4Address*)srcdat->GetRTCPDataAddress();
        ip = addr->GetIP();
        port = addr->GetPort() - 1;
    }
    else {
        LOG(INFO)<<"33";
        return;
    }

    RTPIPv4Address dest(ip, port);
    AddDestination(dest);
    struct in_addr inaddr;
    inaddr.s_addr = htonl(ip);
    LOG(INFO)<<"Adding destination "<<string(inet_ntoa(inaddr))<<":"<<port;
}

void Gb28181Session::OnRemoveSource(RTPSourceData* srcdat) {

}

/**
 *  RTP的BYE包
 */
void Gb28181Session::OnBYEPacket(RTPSourceData* srcdat) {

}

int Gb28181Session::createRtpSession(string dstip, int dstport) {
    LOG(INFO)<<"CreateRtpSession";
    RTPSessionParams sessParams;
    sessParams.SetOwnTimestampUnit(1.0/90000.0);
    sessParams.SetAcceptOwnPackets(true);
    sessParams.SetUsePollThread(true);
    sessParams.SetNeedThreadSafety(true);
    sessParams.SetMinimumRTCPTransmissionInterval(RTPTime(5,0));
	int ret = -1;
    if (protocal == 0) {
        RTPUDPv4TransmissionParams transparams;
        transparams.SetRTPReceiveBuffer(1024 * 1024);
        transparams.SetPortbase(rtp_loaclport);
        
        ret = Create(sessParams, &transparams);
		LOG(INFO)<<"ret:"<<ret;
        if (ret < 0) {
            LOG(ERROR)<<"udp create fail";
        } else {
            LOG(INFO)<<"udp create ok,bind:"<<rtp_loaclport;
        }
    } else {
        sessParams.SetMaximumPacketSize(65535);
        RTPTCPTransmissionParams transParams;
        ret = Create(sessParams, &transParams, RTPTransmitter::TCPProto);
        if (ret < 0) {
            LOG(ERROR) << "Rtp tcp error: " << RTPGetErrorString(ret);
            return -1;
        }

        //会话创建成功后，接下来需要创建tcp连接
        int sessFd = RtpTcpInit(dstip, dstport, 5);
		LOG(INFO)<<"sessFd:"<<sessFd;
        if (sessFd < 0) {
            LOG(ERROR)<<"RtpTcpInit faild";
            return -1;
        } else {
            AddDestination(RTPTCPAddress(sessFd));
        }
    }

    return ret;
}
