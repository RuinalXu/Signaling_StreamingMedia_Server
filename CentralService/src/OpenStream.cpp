#include "OpenStream.h"
#include <sip/SipInclude.h>
#include "Gb28181Session.h"

OpenStream::OpenStream() {
    m_pStreamTimer = new TaskTimer(600);
}

OpenStream::~OpenStream() {}

void OpenStream::StreamServiceStart() {
    if (m_pStreamTimer && m_pCheckSessionTimer) {
        m_pStreamTimer->setTimerFun(OpenStream::StreamGetProc, this);
        m_pStreamTimer->start();
    }
}

/**
 *  实现定时器任务
 */
void OpenStream::StreamGetProc(void* param) {

    // Request INVITE
        // 智能锁缩小栈空间,实际的栈空间就{}之间的部分

    

    // SDP
    
    /* 引用GbSeession */
    Gb28181Session* session = new Gb28181Session();
    
}