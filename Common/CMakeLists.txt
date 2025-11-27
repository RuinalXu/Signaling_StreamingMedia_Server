cmake_minimum_required(VERSION 3.20)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(PJLIB_ROOT "../third_party/pjproject/pjlib")
set(PJSIP_ROOT "../third_party/pjproject/pjsip")
set(PJLIB_UTIL_ROOT "../third_party/pjproject/pjlib-util")
set(PJMEDIA_ROOT "../third_party/pjproject/pjmedia")

# 编译为动态库
add_library(Common STATIC ${COMMON_SRC})

# 编辑时定义宏,指定字节序
target_compile_definitions(Common PRIVATE PJ_IS_LITTLE_ENDIAN=1)

# 添加项目头文件
target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 添加pjproject第三方库头文件
target_include_directories(Common PRIVATE ${PJLIB_ROOT}/include)
target_include_directories(Common PRIVATE ${PJSIP_ROOT}/include)
target_include_directories(Common PRIVATE ${PJLIB_UTIL_ROOT}/include)
target_include_directories(Common PRIVATE ${PJMEDIA_ROOT}/include)

# pjsip相关链接库
find_library(PJLIB_LIB pj-aarch64-unknown-linux-gnu PATHS ${PJLIB_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJSIP_LIB pjsip-aarch64-unknown-linux-gnu PATHS ${PJSIP_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJSIP_UA_LIB pjsip-ua-aarch64-unknown-linux-gnu PATHS ${PJSIP_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJLIB_UTIL_LIB pjlib-util-aarch64-unknown-linux-gnu PATHS ${PJLIB_UTIL_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
# pjmdeia相关链接库
find_library(PJMEDIA_LIB pjmedia-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_LIB pjmedia-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_AUDIODEV pjmedia-audiodev-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_VIDEODEV pjmedia-videodev-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_SDP pjsdp-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PJMEDIA_CODEC pjmedia-codec-aarch64-unknown-linux-gnu PATHS ${PJMEDIA_ROOT}/lib NO_DEFAULT_PATH REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(libSRTP CONFIG REQUIRED)

target_link_libraries(Common PRIVATE
    glog::glog
    gflags::gflags
    fmt::fmt
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
)

target_link_libraries(Common PRIVATE
    # 高层应用逻辑
    ${PJSIP_UA_LIB}
    ${PJSIP_LIB}

    # 媒体相关（pjmedia 及其组件）
    ${PJMEDIA_AUDIODEV}
    ${PJMEDIA_VIDEODEV}
    ${PJMEDIA_CODEC}
    ${PJMEDIA_SDP}
    ${PJMEDIA_LIB}

    # 工具库
    ${PJLIB_UTIL_LIB}

    # 核心基础库（必须最后）
    ${PJLIB_LIB}

    libSRTP::srtp2
)