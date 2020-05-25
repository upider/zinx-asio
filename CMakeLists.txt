cmake_minimum_required(VERSION 3.10)
project(zinx-asio)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -frtti -std=c++11 -Wall -O2")

#json库
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("third_party/json")

find_package(Boost)
include_directories(${PROJECT_SOURCE_DIR}/zinx-asio ${Boost_INCLUDE_DIRS} third_party/json/single_include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

#可执行文件
add_executable(byte_buffer_test src/data_pack.cpp
	src/utils.cpp src/tcp_message.cpp demo/tcp/byte_buffer_test.cpp)
target_link_libraries(byte_buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

add_executable(buffer_test src/data_pack.cpp src/utils.cpp demo/tcp/buffer_test.cpp)
target_link_libraries(buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

add_executable(client src/data_pack.cpp src/utils.cpp demo/tcp/client10.cpp)
target_link_libraries(client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

add_executable(forever_client src/data_pack.cpp src/utils.cpp demo/tcp/forever_client.cpp)
target_link_libraries(forever_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

add_executable(idle_client src/data_pack.cpp src/utils.cpp demo/tcp/idle_client.cpp)
target_link_libraries(idle_client PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

add_executable(server ${SRC_LIST} demo/tcp/server10.cpp)
target_link_libraries(server PRIVATE pthread boost_context boost_coroutine boost_thread nlohmann_json::nlohmann_json)

#输出目录
#EXECUTABLE_OUTPUT_PATH && LIBRARY_OUTPUT_PATH用来指定二进制文件位置和库文件路径，这里不包括中间文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
