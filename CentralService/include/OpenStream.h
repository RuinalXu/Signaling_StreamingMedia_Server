#ifndef OPEN_STREAM_H
#define OPEN_STREAM_H

#include <timer/TaskTimer.h>
#include <string>

/**
 *  上级发送INVITE请求,基上级主动向下级进行开流
 */
class OpenStream {
private:
    TaskTimer* m_pStreamTimer;
    TaskTimer* m_pCheckSessionTimer;
public:
    OpenStream();
    ~OpenStream();

    /**
     *  外部开始接口
     */
    void StreamServiceStart();
    static void StreamGetProc(void* param);
    // static void CheckSession(void* param);
    // static void StreamStop(std::string platformId, std::string devId);
};

#endif
