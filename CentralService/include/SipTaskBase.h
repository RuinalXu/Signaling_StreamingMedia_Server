#ifndef SIP_TASK_BASE
#define SIP_TASK_BASE

class SipTaskBase {
public:
    SipTaskBase(){};
    virtual ~SipTaskBase();

    virtual void run() = 0;
};

#endif
