find_package(GTest REQUIRED) # tell CMAKE to find googleTest, add its include directories, link against it

# Basic test
add_executable(test_database test_buffer_pool.cpp test_disk_manager.cpp test_page.cpp)
target_link_libraries(test_database
    PRIVATE 
    db_core
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_database)