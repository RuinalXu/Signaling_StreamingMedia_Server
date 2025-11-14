#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H


/**
 * 下级注册请求
 */
class SipRegister
{
private:
    TaskTimer m_regTimer;
public:
    SipRegister(/* args */);
    ~SipRegister();
    
    /**
     * 发送注册请求
     */
    int gbRegister();
};

SipRegister::SipRegister(/* args */)
{
}

SipRegister::~SipRegister()
{
}


#endif