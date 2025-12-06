#ifndef SIP_DIRECTORY_H
#define SIP_DIRECTORY_H

#include "SipTaskBase.h"

/**
 *  上级对下级推送的目录树进行校验
 */
class SipDirectory : public SipTaskBase {
private:
    tinyxml2::XMLElement* m_pRootElement;    
public:
    SipDirectory(tinyxml2::XMLElement* root);
    ~SipDirectory();
    virtual pj_status_t run(pjsip_rx_data *rdata);
    void saveDir(int& status_code);
};

#endif