cmake_minimum_required(VERSION 3.1)     # cmake最小版本3.0

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

set(CMAKE_CXX_STANDARD 17) # 设置C++标准为C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 确保C++标准是必须的

find_package(Threads REQUIRED)
add_executable(controller_exe main.cpp ${SRC_LIST} )

target_link_libraries(controller_exe Threads::Threads)


