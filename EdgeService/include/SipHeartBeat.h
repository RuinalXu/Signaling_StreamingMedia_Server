#ifndef SIP_HEART_BEAT_H
#define SIP_HEART_BEAT_H

#include "TaskTimer.h"

class SipHeartBeat {
private:
    TaskTimer* m_heartTimer;
public:
    SipHeartBeat();
    ~SipHeartBeat();

    void gbHeartBeatServiceStart();
    static void HeartBeatProc(void* param);
}

#endif