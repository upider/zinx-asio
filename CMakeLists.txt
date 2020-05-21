cmake_minimum_required(VERSION 3.10)
project(zinx-asio)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O2")

#json库
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("third_party/json")

find_package(Boost)
include_directories(${PROJECT_SOURCE_DIR}/zinx-asio ${Boost_INCLUDE_DIRS} third_party/json/single_include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

add_executable(server ${SRC_LIST} demo/server10.cpp)
add_executable(byte_buffer_test ${SRC_LIST} demo/byte_buffer_test.cpp)
add_executable(bytestream_test ${SRC_LIST} demo/bytestream_test.cpp)
add_executable(buffer_test ${SRC_LIST} demo/buffer_test.cpp)
add_executable(bytestream_test2 ${SRC_LIST} demo/bytestream_test2.cpp)
add_executable(client ${SRC_LIST} demo/client10.cpp)
add_executable(forever_client ${SRC_LIST} demo/forever_client.cpp)
add_executable(idle_client ${SRC_LIST} demo/idle_client.cpp)
target_link_libraries(byte_buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(bytestream_test2 PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(bytestream_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(server PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(forever_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
target_link_libraries(idle_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)
