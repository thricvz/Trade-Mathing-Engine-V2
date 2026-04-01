

add_library(ProRataLib ProRata.cpp)
target_link_libraries(ProRataLib FifoLib)

add_subdirectory(ProRataTests)
