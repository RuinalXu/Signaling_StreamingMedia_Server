#ifndef GET_CATALOG_H
#define GET_CATALOG_H

/**
 *  上级发送目录树请求
 */
class GetCatalog {
public:
    GetCatalog();
    ~GetCatalog();

    void directoryGetPro(void* param);
};

#endif
