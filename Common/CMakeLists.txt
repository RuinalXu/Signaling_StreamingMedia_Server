cmake_minimum_required(VERSION 3.20)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(PJPROJECT_PJLIB_ROOT "../third_party/pjproject/pjlib")
# set(PJPROJECT_PJSIP_ROOT "../third_party/pjproject/pjsip")

# 编译为动态库
add_library(Common STATIC ${COMMON_SRC})

# 编辑时定义宏,指定字节序
target_compile_definitions(Common PRIVATE PJ_IS_LITTLE_ENDIAN=1)

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Common PRIVATE ${PJPROJECT_PJLIB_ROOT}/include)
# target_include_directories(Common PRIVATE ${PJPROJECT_PJSIP_ROOT}/include)

find_library(PJLIB_LIB pj-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJLIB_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
# find_library(PJSIP_LIB pjsip-aarch64-unknown-linux-gnu PATHS ${PJPROJECT_PJSIP_ROOT}/lib NO_DEFAULT_PATH REQUIRED)

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

# target_link_libraries(Common PRIVATE
#     ${PJSIP_LIB}
#     ${PJLIB_LIB}
# )

target_link_libraries(Common PRIVATE
    ${PJLIB_LIB}
)