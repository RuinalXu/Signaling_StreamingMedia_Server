#include "timer/TaskTimer.h"
#include "thread/ECThread.h"
#include "log/LogManager.h"
#include <pjlib.h>

using namespace EC;

TaskTimer::TaskTimer(int timeSecond)
: m_timerFun(NULL), m_funParam(NULL), m_timeSecond(timeSecond), m_timerStop(false){}

TaskTimer::~TaskTimer() {
    stop();
}

/**
 *  创建线程入口函数
 */
void TaskTimer::start() {
    pthread_t pid;
    int ret = EC::ECThread::createThread(TaskTimer::timer, (void*)this, pid);
    if (ret != 0) {
        LOG(ERROR) << "TaskTimer create thread failed";
    }
}

void TaskTimer::stop() {
    m_timerStop = true;
}

/**
 *  设置定时任务的对应的函数指针(回调函数)和参数
 */
void TaskTimer::setTimerFun(timerCallBack fun, void* param) {
    m_timerFun = fun;
    m_funParam = param;
}

/**
 *  TaskTimer定时器的线程入口函数
 */
void* TaskTimer::timer(void* context) {
    // 计算时间，到时间后调用成员指针m_timerFun去执行用户指定的任务
    TaskTimer* pthis = (TaskTimer*)context;
    if (pthis == NULL) {
        return NULL;
    }

    // 当前时间
    unsigned long curTm = 0;

    // 最后更新时间
    unsigned long lastTm = 0;

    while (!(pthis->m_timerStop)) {
        struct timeval current;
        gettimeofday(&current, NULL);
        curTm = current.tv_sec * 1000 + current.tv_usec / 1000;
        // 当前时间与最后更新的时间大于指定时间
        if ((curTm - lastTm) >= (pthis->m_timeSecond * 1000)) {
            // 到达定时时间后，先更新lastTm
            lastTm = curTm;
            if (pthis->m_timerFun != NULL) {
                // TODO:(可优化)将TaskTimer线程进行注册
                if (!pj_thread_is_registered()) {
                    pj_thread_desc desc;
                    pj_thread_t* thread = 0;
                    pj_status_t status;
                    status = pj_thread_register(NULL, desc, &thread);
                    if (status != PJ_SUCCESS) {
                        LOG(ERROR) << "thread register error";
                    }
                }
                pthis->m_timerFun(pthis->m_funParam);
            }
        } else {
            // 未到达延时1s
            usleep(1000 * 1000);
            continue;
        }
    }
    return NULL;
}