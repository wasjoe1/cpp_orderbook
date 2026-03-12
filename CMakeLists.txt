cmake_minimum_required(VERSION 3.20)

project(orderbook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find fmt from vcpkg
find_package(fmt CONFIG REQUIRED)

# create executable
add_executable(orderbook
    src/orderbook.cpp
)

# link library
target_link_libraries(orderbook PRIVATE fmt::fmt)