
add_executable(FifoTestsExec FifoTestsMain.cpp)
target_link_libraries(FifoTestsExec FifoLib GTest::gtest_main)

add_test(NAME FifoAlgorithmTest COMMAND $<TARGET_FILE:FifoTestsExec>)
