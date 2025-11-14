#ifndef TASK_TIMER_H
#define TASK_TIMER_H

#include <unistd.h>
#include <sys/time.h>

/**
 * 回调函数指针
 */
typedef void (*timerCallBack)(void* param);

class TaskTimer
{
private:
    /**
     * 定时任务方法
     */
    timerCallBack m_timerFun;

    /**
     * 函数回调参数
     */
    void* m_funParam;

    /**
     * 定时任务时间
     */
    int m_timeSecond;

    bool m_timerStop;
public:
    TaskTimer(int timeSecond);
    ~TaskTimer();

    void start();
    void stop();

    void setTimerFun(timerCallBack fun, void* param);

    /**
     * 定时线程的入口方法
     */
    static void* timer(void* context);
};




#endif
