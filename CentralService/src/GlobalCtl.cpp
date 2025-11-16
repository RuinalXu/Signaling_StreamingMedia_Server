#include "GlobalCtl.h"
#include <random>
#include <sstream>

GlobalCtl* GlobalCtl::m_pInstance = NULL;

GlobalCtl* GlobalCtl::instance() {
    if (!m_pInstance) {
        m_pInstance = new GlobalCtl();
    }
    return m_pInstance;
}

bool GlobalCtl::init(void* param) {
    gConfig = (SipLocalConfig*) param;
    if (!gConfig) {
        return false;
    }

    if (!gThreadPool) {
        gThreadPool = new ThreadPool();
        gThreadPool -> createThreadPool(10);
    }

    if (!gSipServer) {
        gSipServer = new SipCore();
    }
    gSipServer -> initSip(gConfig -> sipPort())

    return true;
}

std::string GlobalCtl::randomNum(int length){
    // 随机数种子
    random_device rd;
    // 随机数生成器
    mt19937 gen(rd());
    // 分布器类模版,设置1个0-15的均匀整数分布的区间范围
    uniform_int_distribution<> dia(0, 15);
    stringstream ss;
    for (int i= 0; i < length; i++){
        // 每次使用随机数生成器在指定的分布范围内获取一个随机数
        int value = dis(gen);
        ss << std::hex << value;
    }
    LOG(INFO) << "random: " << ss.str();
    return ss.str();
}
