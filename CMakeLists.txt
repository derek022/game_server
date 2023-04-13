cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)

find_package(Boost REQUIRED)
if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
endif()

find_library(YAMLCPP yaml-cpp)

set(LIB_SRC 
    sylar/log.cc
    sylar/util.cc
    sylar/thread.cc
    sylar/config.cc
    sylar/mutex.cc
    )

add_library(sylar SHARED ${LIB_SRC})

# test
add_executable(test tests/test.cc)
add_dependencies(test sylar)
target_link_libraries(test sylar ${YAMLCPP})

#test_config
add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar ${YAMLCPP})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
