#ifndef SIP_HEART_BEAT_H
#define SIP_HEART_EEAT_H

#include <timer/TaskTimer.h>
#include "GlobalCtl.h"

/**
 *  下级向上级发送的心跳包
 */
class SipHeartBeat {
private:
    TaskTimer* m_heartTimer;
    static int m_snIndex;
public:
    SipHeartBeat();
    ~SipHeartBeat();
    void gbHeartBeatServiceStart();
    static void HeartBeatProc(void* param);
    int gbHeartBeat(GlobalCtl::SupDomainInfo& node);
};

#endif
