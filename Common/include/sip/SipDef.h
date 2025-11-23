#ifndef SIP_DEF_H
#define SIP_DEF_H

#include <string>
#include <string.h>  

#define SIP_STACK_SIZE      1024*256

// endpoint所分配的内存池空间大小
#define SIP_ALLOC_POOL_1M   1024*1024*1

/**
 *  自定义状态码
 */
enum statusCode{
    SIP_SUCCESS = 200,
    SIP_BADREQUEST = 400,  //请求的参数或者格式不对，请求非法
    SIP_FORBIDDEN = 404,   //请求的用户在本域中不存在

};

#endif
