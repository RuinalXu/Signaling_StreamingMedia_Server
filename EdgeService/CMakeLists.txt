cmake_minimum_required(VERSION 3.20)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译后的可执行名称并指定变量名
set(EXE_NAME EdgeService)

# 判断当前使用的操作系统
message(STATUS "Operation System is ${CMAKE_SYSTEM}")

# 在编译选项中开启所有警告和添加C++11支持
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")

# 在编译时使用-g选项添加调试信息，编译后的程序可以使用gdb调试
add_definitions(-g)

find_package(fmt CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)

# 指定需要编译的源文件路径，并指定相应的变量名
aux_source_directory(./src SRC)
include_directories(./include)
# 大模型推荐：使用现代CMake风格，避免全局的include_directories
# target_include_directories(${EXE_NAME} PRIVATE ./include)

add_executable(${EXE_NAME} ${SRC})

target_link_libraries(${EXE_NAME} PRIVATE
    fmt::fmt
    glog::glog
    gflags::gflags
    tinyxml2::tinyxml2
    JsonCpp::JsonCpp
    libevent::extra
    libevent::pthreads
)


