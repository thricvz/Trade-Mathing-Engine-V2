cmake_minimum_required(VERSION 3.26)
project(TradeMatchingEngine)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

  

include_directories(Orderbook/Include)
include_directories(Json)

add_subdirectory(Orderbook)
add_subdirectory(DataBase)
add_subdirectory(RessourceManager)
