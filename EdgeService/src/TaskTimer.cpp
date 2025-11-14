#include "TaskTimer.h"
#include "ECThread.h"
#include "common.h"

using namespace EC;

TaskTimer::TaskTimer(int timeSecond){
    m_timeSecond = timeSecond;
    m_timerFun = NULL;
    m_funParam = NULL;
    m_timerStop = false;
}

TaskTimer::~TaskTimer(){
    stop();
}

/**
 * 启动线程执行相关任务
 */
void TaskTimer::start(){
    pthread_t pid;
    int ret = EC::ECThread::createThread(TaskTimer::timer, (void*)this, pid);
    if (ret != 0){
        LOG(ERROR) << "create thread fialed";
    }
}


void TaskTimer::stop(){
    m_timerStop = true;
}

void TaskTimer::setTimerFun(timerCallBack fun, void* param){
    m_timerFun = fun;
    m_funParam = param;
}

/**
 * 到达指定时间后,将会调用用户指定的函数指针指向的函数
 */
void* TaskTimer::timer(void* context){
    TaskTimer* pthis = (TaskTimer*)context;
    if (pthis == NULL){
        return NULL;
    }

    unsigned long curTm = 0;
    unsigned long lastTm = 0;
    while (!pthis->m_timerStop){
        struct timeval current;
        gettimeofday(&current, NULL);

        // 当前时间换算为毫秒
        curTm = current.tv_sec * 1000 + current.tv_usec / 1000;

        if ((curTm - lastTm) >= ((pthis->m_timeSecond) * 1000)){
            lastTm = curTm;
            if (pthis->m_timerFun == NULL){
                LOG(ERROR) << "timerFun is NULL";
            }
            pthis->m_timerFun(pthis->m_funParam);
        } else {
            usleep(1000 * 1000);
            continue;
        }
    }
    return NULL;
}