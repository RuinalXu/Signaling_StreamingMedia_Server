#include "SipRegister.h"

SipRegister::SipRegister(){

}

SipRegister::~SipRegister(){

}

void SipRegister::run(pjsip_rx_data *rdata){
    return RegisterRequestMessage(rdata);
}

pj_status_t SipRegister::RegisterRequestMessage(pjsip_rx_data *rdata){
    
}

