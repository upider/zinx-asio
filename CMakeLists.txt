cmake_minimum_required(VERSION 3.10)
project(zinx-asio)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O2")
#json库
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

find_package(Boost)
include_directories(zinx-asio ${Boost_INCLUDE_DIRS})

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(server ${SRC_LIST} demo/server10.cpp)
add_executable(bytes_test ${SRC_LIST} demo/bytes_test.cpp)
add_executable(bytes_test2 ${SRC_LIST} demo/bytes_test2.cpp)
add_executable(client ${SRC_LIST} demo/client10.cpp)
add_executable(forever_client ${SRC_LIST} demo/forever_client.cpp)
add_executable(idle_client ${SRC_LIST} demo/idle_client.cpp)
target_link_libraries(bytes_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(bytes_test2 PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(server PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(forever_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(idle_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
