cmake_minimum_required(VERSION 3.20)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# 编译为动态库
add_library(Common STATIC ${COMMON_SRC})

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(fmt CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

target_link_libraries(Common PRIVATE
    glog::glog
    gflags::gflags
    fmt::fmt
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
)