CMake的新的版本支持使用json文件作为命令行的预设置（≥ 3.19），免去了在命令行写过长的命令行参数，但是需要注意CMake命令行参数与preset设置之间的冲突，虽然cmake设置这两者同时存在时所选择的优先级，但是实际过程中不要冲突使用，以免造成未知错误或对项目排除造成困扰；

之前所使用的cmake编译命令：`cmake -B ./build -S ./ --preset user_preset`；
- `-B`：或使用全名`--build`，告诉cmake把生成的构建文件（如 Makefile、.ninja、编译中间文件等）存放在哪里；
- `-S`：或使用全名`--source`，告诉cmake项目源代码在哪里（即包含`CMakeLists.txt`的目录）；

CMAKE_MAKE_PROGRAM应该指向 make 或 ninja，而不是 CMake 自己