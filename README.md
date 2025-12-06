# 基于GB28181的信令流媒体服务器

最终实现目的：按照GB28181手册，从零开发并完成上下级信令服务器和媒体服务器。

[飞书云文档开发博客](https://icnob5v18inr.feishu.cn/wiki/UWNzwl1IiiiFKakq2Aeckw1pnGb?from=from_copylink)
---

## 开发环境：
- Ubuntu Server 22.4
- CMake项目构建 + vcpkg管理部分第三方库 + tcpdump（抓包）
- wireshark（分析所抓包信息）

## 所用到的第三方库
vcpkg精选注册表端口：
1. jsoncpp：处理json文件的库；
2. timyxml2：处理XML文档的库；
3. glog：Google风格的日志库;
4. gflags：命令行参数/标志（flags）解析库；
5. libevent：高性能、跨平台的事件驱动网络库；
6. ffmpeg

非精选注册表端口：
1. pjproject：与SIP信令控制相关的库；
2. jrtplib：与流媒体传输与控制相关的库；

# 项目的架构

整个服务端项目分为上下级开发，上级服务（CentralService）和下级服务（EdgeService），其中重构公共模块（Common），便于上下级代码的重用。





