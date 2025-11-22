#ifndef TASK_TIMER_H
#define TASK_TIMER_H

#include <unistd.h>
#include <sys/time.h>

/* 执行定时任务的函数指针 */
typedef void (*timerCallBack)(void* param);

/**
 *  定时开启线程执行
 */
class TaskTimer {
private:
    /**
     *  回调参数指针
     */
    timerCallBack m_timerFun;

    /**
     *  回调参数
     */
    void* m_funParam;
    
    /**
     *  定时时间
     */
    int m_timeSecond;
    
    /**
     *  定时器结束时机
     */
    bool m_timerStop;
public:
    TaskTimer(int timeSecond);
    ~TaskTimer();

    void start();
    void stop();
    void setTimerFun(timerCallBack fun, void* param);

    /**
     *  定时任务线程入口
     */
    static void* timer(void* context);
};

#endif
