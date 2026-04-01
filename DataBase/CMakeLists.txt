
find_package(SQLite3 REQUIRED)
include_directories(Include)

add_library(DataBaseLib DataBase.cpp)
add_executable(main test_main.cpp)

target_link_libraries(DataBaseLib SQLite::SQLite3)
target_link_libraries(main DataBaseLib)

