cmake_minimum_required(VERSION 3.26)
project(TradeMatchingEngine)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

  

include_directories(Json)
include_directories(Orderbook/Include)
include_directories(Orderbook/Include/Fifo)
include_directories(DataBase/Include)
include_directories(RessourceManager/Include)

add_subdirectory(Orderbook)
add_subdirectory(DataBase)
add_subdirectory(RessourceManager)
add_subdirectory(HttpServer)


#add_executable(main main.cpp)
#target_link_libraries(main RessourceManagerLib)
