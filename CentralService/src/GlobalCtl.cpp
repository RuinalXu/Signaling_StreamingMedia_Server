#include "GlobalCtl.h"

GlobalCtl* GlobalCtl::m_pInstance = NULL;
GlobalCtl::SUBDOMAININFOLIST GlobalCtl::subDomainInfoList;
pthread_mutex_t GlobalCtl::globalLock = PTHREAD_MUTEX_INITIALIZER;
bool GlobalCtl::gStopPoll = false;

GlobalCtl* GlobalCtl::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalCtl();
    }
    return m_pInstance;
}

/**
 *  初始化函数,配置信息,endpoint对象
 */
bool GlobalCtl::init(void* param) {
    gConfig = (SipLocalConfig*)param;
    if (!gConfig) {
        return false;
    }

    // 当本地配置初始化成功后进行supDomainInfoList的初始化
    SubDomainInfo info;
    auto iter = gConfig->subNodeInfoList.begin();
    for (;iter != gConfig->subNodeInfoList.end(); ++iter) {
        info.sipId = iter->id;
        info.addrIp = iter->ip;
        info.sipPort = iter->port;
        info.protocal = iter->poto;
        info.auth = iter->auth;
        subDomainInfoList.push_back(info);
    }
    LOG(INFO)<<"subDomainInfoList.SIZE:"<<subDomainInfoList.size();

    if (!gThPool) {
        gThPool = new ThreadPool();
        gThPool->createThreadPool(10);
    }
    
    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer->InitSip(gConfig->sipPort());

    return true;
}

/**
 *  查询下级节点是否在列表中
 */
 bool GlobalCtl::checkIsExist(string id) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    LOG(ERROR) << "id = " << id;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        return true;
    }
    return false;
 }

/**
 *  设置下级的生存周期
 */
 void GlobalCtl::setExpires(string id, int expires) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        it->expires = expires;
    }
 }

/**
 *  判断下级注册时间是否过期
 */
void GlobalCtl::setRegister(string id, bool registered) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        it->registered = registered;
    }
}

/**
 *  设置最后注册时间
 */
void GlobalCtl::setLastRegTime(string id, time_t t) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        it->lastRegTime = t;
    }
}

/**
 *  获取随机数
 */
string GlobalCtl::randomNum(int length) {
    #if 0
    //随机数种子
    random_device rd;
    //随机数生成器
    mt19937 gen(rd());
    //分布器类模板  设定一个0-15的均匀整数分布的区间范围
    uniform_int_distribution<> dis(0,15);
    stringstream ss;
    for(int i =0;i<length;++i)
    {
        //每次使用随机数生成器在指定的分布范围内获取一个随机数
        int value = dis(gen);
        ss<< std::hex << value;
    }
    #endif
    stringstream ss;
    for (int i =0;i<length;++i) {   
        int value = random() % 15;
        ss<< std::hex << value;
    }
    return ss.str();
}

bool GlobalCtl::getAuth(string id) {
    AutoMutexLock lck(&globalLock);
    SUBDOMAININFOLIST::iterator it;
    it = std::find(subDomainInfoList.begin(), subDomainInfoList.end(), id);
    if (it != subDomainInfoList.end()) {
        return it->auth;
    }
    return false;
}