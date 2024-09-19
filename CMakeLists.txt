cmake_minimum_required(VERSION 3.10)
project(file_encryption_tool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)

add_executable(file_encryption_tool
    src/main.cpp
    src/encryption.cpp
)

target_include_directories(file_encryption_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(file_encryption_tool PRIVATE OpenSSL::Crypto)