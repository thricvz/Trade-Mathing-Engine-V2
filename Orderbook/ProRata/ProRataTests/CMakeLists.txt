
add_executable(ProRataTestsExec ProRataTestsMain.cpp)

target_link_libraries(ProRataTestsExec ProRataLib GTest::gtest_main)

# add_test(NAME ProRataAlgorithmTest COMMAND $<TARGET_FILE:ProRataTestsExec>)
