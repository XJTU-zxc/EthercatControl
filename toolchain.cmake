set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
# 定义 tools 变量为绝对路径  
set(tools /home/zxc/RK3568/rk356x_linux_release_v1.3.1_20221120/buildroot/output/rockchip_rk3568)  

# 设置交叉编译器  
set(CMAKE_C_COMPILER ${tools}/host/bin/aarch64-buildroot-linux-gnu-gcc)  
set(CMAKE_CXX_COMPILER ${tools}/host/bin/aarch64-buildroot-linux-gnu-g++)  

# 设置 sysroot 路径  
set(CMAKE_FIND_ROOT_PATH  ${tools}/target)  

# 选项：设置查找模式  
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  # 不在 sysroot 中查找程序
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)   # 在 sysroot 和主机路径中查找库  
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)  