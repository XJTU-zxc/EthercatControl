cmake_minimum_required(VERSION 3.10)     # cmake最小版本3.10

# 定义 ec_tools 变量为绝对路径  
# 如果使用交叉编译，则设置工具链文件
option(USE_CROSS "使用交叉编译" OFF)
if(USE_CROSS)
    set(ec_tools /home/zxc/EtherCAT/ethercat/_install/RK3568) 
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake) # 设置交叉编译工具链文件
else()
    set(ec_tools /home/zxc/EtherCAT/ethercat/_install/Ubuntu) 
endif()


project(controller)   # 项目名称

set(CMAKE_BUILD_TYPE Debug) # 生成可调试的可执行文件

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

include_directories(include)
include_directories(comp/accLineFloat)
include_directories(comp/accLSFloat)
include_directories(comp/accLSint)
include_directories(comp/accMulAxis)
include_directories(comp/accSFloat)
include_directories(comp/accSFloat2)
include_directories(comp/accSfloatVs)
include_directories(comp/accSint)
include_directories(comp/circle)
include_directories(comp/jog)
include_directories(comp/rapidMove)
include_directories(comp/returnRef)
include_directories(comp/swing)
include_directories(lib)
include_directories(lib/MotionControl)
include_directories(lib/ProgrammerControl)

include_directories(${ec_tools}/include)

file(GLOB_RECURSE SRC_LIST 
     "src/*.cpp"
     "src/*.c"
     "comp/accLineFloat/*.cpp"
     "comp/accLSFloat/*.cpp"
     "comp/caccLSint/*.cpp"
     "comp/accMulAxis/*.cpp"
     "comp/accSFloat/*.cpp"
     "comp/accSFloat2/*.cpp"
     "comp/accSfloatVs/*.cpp"
     "comp/accSint/*.cpp"
     "comp/circle/*.cpp"
     "comp/jog/*.cpp"
     "comp/rapidMove/*.cpp"
     "comp/returnRef/*.cpp"
     "comp/swing/*.cpp"
     "lib/*.cpp"
     "lib/*.c"
     "lib/MotionControl/*.cpp"
     "lib/ProgrammerControl/*.cpp")

# set(CMAKE_CXX_FLAGS "-g -Wall -O2")  # 添加编译选项
set(CMAKE_CXX_STANDARD 17) # 设置C++标准为C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 确保C++标准是必须的                             
find_package(Threads REQUIRED)
link_directories(${ec_tools}/lib) # 链接库目录
# set(CMAKE_CXX_FLAGS "-g -Wall -O2")  # 添加编译选项
set(LIBS -lethercat)  # 链接库
add_executable(controller_exe main.cpp ${SRC_LIST} )
target_link_libraries(controller_exe PRIVATE ${LIBS} Threads::Threads) # 对可执行文件进行链接



