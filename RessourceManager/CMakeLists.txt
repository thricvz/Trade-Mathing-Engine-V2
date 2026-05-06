include_directories(Include)


add_library(RessourceManagerLib RessourceManager.cpp) 
target_link_libraries(RessourceManagerLib DataBaseLib OrderbookLib)

add_executable(RessourceManagerTests tests.cpp)
target_link_libraries(RessourceManagerTests GTest::gtest_main RessourceManagerLib)

enable_testing()
add_test(NAME RessourceManagerTest COMMAND $<TARGET_FILE:RessourceManagerTests>)
