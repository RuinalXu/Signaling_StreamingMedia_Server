#ifndef SIP_STATUS_CODE_H
#define SIP_STATUS_CODE_H

/**
 *  自定义状态码
 */
enum statusCode{
    SIP_SUCCESS = 200,
    SIP_BADREQUEST = 400,  //请求的参数或者格式不对，请求非法
    SIP_FORBIDDEN = 404,   //请求的用户在本域中不存在

};

#endif