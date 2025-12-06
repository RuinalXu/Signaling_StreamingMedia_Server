#ifndef GB28181_SESSION_H
#define GB28181_SESSION_H

#include "rtpsession.h"
#include "rtpsourcedata.h"
#include "rtptcptransmitter.h"
#include "rtptcpaddress.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtcpsrpacket.h"
#include "mpeg-ps.h"
#include <sip/SipDef.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "jthread.h"
#ifdef __cplusplus
}
#endif

#include "GlobalCtl.h"

using namespace jrtplib;

/**
 *  下级RTP会话控制
 */
class Gb28181Session : public RTPSession {

protected:
    void OnNewSource(RTPSourceData* srcdat);
    void OnRemoveSource(RTPSourceData* srcdat);
    void OnBYEPacket(RTPSourceData* srcdat);
public:
    Gb28181Session();
    ~Gb28181Session();

    int createRtpSession(string dstip, int dstport);
};

#endif
