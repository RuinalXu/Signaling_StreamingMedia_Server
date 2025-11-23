#ifndef SIP_HEART_BEAT_H
#define SIP_HEART_EEAT_H

#include "GlobalCtl.h"
#include "TaskTimer.h"

/**
 *  下级向上级发送的心跳包
 */
class SipHeartBeat {
private:
    TaskTimer* m_heartTimer;
public:
    SipHeartBeat();
    ~SipHeartBeat();
    void gbHeartBeatServiceStart();
    static void HeartBeatProc(void* param);
    int gbHeartBeat(GlobalCtl::SupDomainInfo& node);
};

#endif
