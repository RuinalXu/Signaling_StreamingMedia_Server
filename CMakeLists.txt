cmake_minimum_required(VERSION 3.20)

project(SipSupService VERSION 1.0.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 公共模块
add_subdirectory(Common)

# 上级服务模块
add_subdirectory(CentralService)

# 下级服务模块
add_subdirectory(EdgeService)