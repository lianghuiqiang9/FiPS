cmake_minimum_required(VERSION 3.10)
project(FiPS_Example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(FiPS)


add_executable(fips_demo main.cpp)

target_include_directories(fips_demo PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/FiPS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FiPS/extlib/ips2ra/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FiPS/extlib/ips4o/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FiPS/extlib/util/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FiPS/extlib/tlx
)

target_link_libraries(fips_demo PRIVATE FiPS::fips)