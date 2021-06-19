cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the project name
project(PAXOS)

include_directories(src /opt/homebrew/Cellar/fmt/7.1.3/include /opt/homebrew/Cellar/spdlog/1.8.5/include)


# add the executable
add_executable(main main.cpp src/node.cpp)

find_library(FMT fmt /opt/homebrew/Cellar/fmt/7.1.3/lib)
find_library(SPDLOG spdlog /opt/homebrew/Cellar/spdlog/1.8.5/lib)
target_link_libraries(main LINK_PUBLIC ${FMT} ${SPDLOG})