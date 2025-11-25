cmake_minimum_required(VERSION 3.20)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置编译后的可执行名称并指定变量名
set(EXE_NAME CentralService)

# 指定需要编译的源文件路径，并指定相应的变量名
aux_source_directory(./src SRC)

add_executable(${EXE_NAME} ${SRC})

set(PJPROJECT_PJLIB_ROOT "../third_party/pjproject/pjlib")
set(PJPROJECT_PJSIP_ROOT "../third_party/pjproject/pjsip")
set(PJPROJECT_PJLIB_UTIL_ROOT "../third_party/pjproject/pjlib-util")
set(PJPROJECT_PJMEDIA_ROOT "../third_party/pjproject/pjmedia")
set(PJPROJECT_PJNATH_ROOT "../third_party/pjproject/pjnath")


# 添加项目头文件以及第三方库头文件
target_include_directories(${EXE_NAME} PRIVATE ./include)
target_include_directories(${EXE_NAME} PRIVATE ${PJPROJECT_PJLIB_ROOT}/include)
target_include_directories(${EXE_NAME} PRIVATE ${PJPROJECT_PJSIP_ROOT}/include)
target_include_directories(${EXE_NAME} PRIVATE ${PJPROJECT_PJLIB_UTIL_ROOT}/include)
target_include_directories(${EXE_NAME} PRIVATE ${PJPROJECT_PJMEDIA_ROOT}/include)
target_include_directories(${EXE_NAME} PRIVATE ${PJPROJECT_PJNATH_ROOT}/include)

# 编辑时定义宏,指定字节序
target_compile_definitions(${EXE_NAME} PRIVATE PJ_IS_LITTLE_ENDIAN=1)

# 判断当前使用的操作系统
message(STATUS "Operation System is ${CMAKE_SYSTEM}")

# 在编译选项中开启所有警告和添加C++11支持
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")

# 在编译时使用-g选项添加调试信息，编译后的程序可以使用gdb调试
add_definitions(-g)

# 查找所需要的pjproject库
find_library(PJLIB_LIB pj-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJLIB_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJSIP_LIB pjsip-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJSIP_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJSIP_UA_LIB pjsip-ua-aarch64-unknown-linux-gnu PATHS ${PJSIP_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJLIB_UTIL_LIB pjlib-util-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJLIB_UTIL_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
# pjmdeia相关连接库
find_library(PJMEDIA_LIB pjmedia-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_AUDIODEV pjmedia-audiodev-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_VIDEODEV pjmedia-videodev-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_SDP pjsdp-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_CODEC pjmedia-codec-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
# pjnath相关链接库
find_library(PJNATH_LIB pjnath-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJNATH_ROOT}/lib NO_DEFAULT_PATH REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)
find_package(libSRTP CONFIG REQUIRED)

target_link_libraries(${EXE_NAME} PRIVATE
    fmt::fmt
    glog::glog
    gflags::gflags
    tinyxml2::tinyxml2
    JsonCpp::JsonCpp
    libevent::extra
    libevent::pthreads
)

target_link_libraries(${EXE_NAME} PRIVATE
    # 高层应用逻辑
    ${PJSIP_UA_LIB}
    ${PJSIP_LIB}

    # 媒体相关（pjmedia 及其组件）
    ${PJMEDIA_AUDIODEV}
    ${PJMEDIA_VIDEODEV}
    ${PJMEDIA_CODEC}
    ${PJMEDIA_SDP}
    ${PJMEDIA_LIB}

    # 网络/NAT 穿透
    ${PJNATH_LIB}

    # 工具库
    ${PJLIB_UTIL_LIB}

    # 核心基础库（必须最后）
    ${PJLIB_LIB}

    libSRTP::srtp2
)

# 引入自定义静态库
target_link_libraries(${EXE_NAME} PRIVATE Common)
