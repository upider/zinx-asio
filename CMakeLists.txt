cmake_minimum_required(VERSION 3.10)
project(zinx-asio)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O2")

find_package(Boost)
include_directories(${PROJECT_SOURCE_DIR}/zinx-asio ${Boost_INCLUDE_DIRS})

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

#udp
add_executable(udp_client src/utils.cpp demo/udp/client)
target_link_libraries(udp_client PRIVATE pthread boost_coroutine boost_thread boost_system)

add_executable(udp_server ${SRC_LIST} demo/udp/server.cpp)
target_link_libraries(udp_server  PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

#tcp
add_executable(byte_buffer_test src/data_pack.cpp
	src/utils.cpp src/tcp_message.cpp demo/tcp/byte_buffer_test.cpp)
target_link_libraries(byte_buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

add_executable(buffer_test src/data_pack.cpp src/utils.cpp demo/tcp/buffer_test.cpp)
target_link_libraries(buffer_test PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

add_executable(tcp_client src/data_pack.cpp src/utils.cpp demo/tcp/client.cpp)
target_link_libraries(tcp_client PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

add_executable(forever_client src/data_pack.cpp src/utils.cpp demo/tcp/forever_client.cpp)
target_link_libraries(forever_client PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

add_executable(idle_client src/data_pack.cpp src/utils.cpp demo/tcp/idle_client.cpp)
target_link_libraries(idle_client PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

add_executable(tcp_server ${SRC_LIST} demo/tcp/server.cpp)
target_link_libraries(tcp_server PRIVATE pthread boost_context boost_coroutine boost_thread boost_system)

#输出目录
#EXECUTABLE_OUTPUT_PATH && LIBRARY_OUTPUT_PATH用来指定二进制文件位置和库文件路径，这里不包括中间文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
