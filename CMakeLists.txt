cmake_minimum_required(VERSION 3.10)
project(zinx-asio)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O2")
#json库
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

find_package(Boost)
include_directories(zinx-asio ${Boost_INCLUDE_DIRS})

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(client ${SRC_LIST} demo/client09.cpp)
add_executable(server ${SRC_LIST} demo/server09.cpp)
target_link_libraries(client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(server PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
