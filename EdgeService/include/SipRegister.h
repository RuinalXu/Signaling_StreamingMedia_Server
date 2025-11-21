#ifndef SIP_REGISTER_H
#define SIP_REGISTER_H

#include "GlobalCtl.h"

class SipRegister {
private:
    
public:
    SipRegister();
    ~SipRegister();
    int gbRegister(GlobalCtl::SupDomainInfo& node);
};

#endif
