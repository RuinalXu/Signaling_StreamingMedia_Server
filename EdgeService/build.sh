#!/bin/sh

# 脚本缺点：
# 1 每次使用该脚本都需要重新下载第三方库；

# 判断当前工作目录中是否存在 build 目录，若存在则删除后再创建；若不存直接创建
if [ -d "./build" ]; then
    echo "==> Removing existing build directory..."
    rm -rf ./build
    echo "==> Creating build directory..."
else 
    echo "==> Creating build directory..."
    mkdir ./build
fi

# 使用预设的配置构建项目
cmake -B ./build -S ./ --preset user_preset

# 编译得到可执行文件
cmake --build ./build