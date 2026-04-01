cmake_minimum_required(VERSION 3.5)
project("Orderbook V2.0")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE DEBUG)
add_compile_options(-Werror -Wpedantic)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(include/)

add_subdirectory(Fifo)
add_subdirectory(ProRata)
add_subdirectory(Tests)

add_library(OrderbookLib OrderBook.cpp)
target_link_libraries(OrderbookLib FifoLib)
