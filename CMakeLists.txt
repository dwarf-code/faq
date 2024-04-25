cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(faq)
find_package(Crow)
set(CROW_FEATURES "ssl;compression")
find_package(OpenSSL REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}/include")
add_executable(faq main.cpp)
target_link_libraries(faq PUBLIC Crow::Crow
OpenSSL::SSL
OpenSSL::Crypto
pthread
dl)
