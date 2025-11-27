#ifndef SIP_DIRECTORY_H
#define SIP_DIRECTORY_H

#include "SipTaskBase.h"

class SipDirectory : public SipTaskBase {
private:
    tinyxml2::XMLElement* m_pRootElement;    
public:
    SipDirectory(tinyxml2::XMLElement* root);
    ~SipDirectory();
    virtual pj_status_t run(pjsip_rx_data *rdata);
    void SaveDir(int& status_code);
};

#endif