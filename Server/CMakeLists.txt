cmake_minimum_required (VERSION 3.8)

project ("Server")

set(CMAKE_CXX_STANDARD 14)

add_executable (Server "ServerW.cpp")

find_package(OpenSSL REQUIRED)

target_include_directories(Server PUBLIC "../../libraries/boost_1_78_0/")

target_link_libraries(Server  /usr/lib/x86_64-linux-gnu/libpthread.so /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1)
