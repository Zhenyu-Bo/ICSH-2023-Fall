cmake_minimum_required(VERSION 3.5)

project(lc3simulator)

set (CMAKE_CXX_STANDARD 17)

SET(CMAKE_BUILD_TYPE "Debug")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
SET(BOOST_ROOT "F:/Download/boost_1_70_0")
set(Boost_DEBUG ON) #是否开启Boost Debug模式，ON位开启，OFF关闭。开启之后会输出很多调试信息，方便寻找错误
set(Boost_ARCHITECTURE "-x64") # CPU架构，对应库文件文件名里的x32或者x64
set(Boost_THREADAPI "win32") # 线程类型，对应thread模块文件名里面的pthread或者win32
find_package(Boost 1.45.0 COMPONENTS program_options)

set(SOURCES
    src/main.cpp
    src/memory.cpp
    src/register.cpp
    src/simulator.cpp)

add_executable(lc3simulator ${SOURCES})

target_include_directories(lc3simulator
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(lc3simulator ${Boost_LIBRARIES})
endif()