
find_package(SQLite3 REQUIRED)
include_directories(Include)

add_library(DataBaseLib DataBase.cpp)

target_link_libraries(DataBaseLib SQLite::SQLite3)

