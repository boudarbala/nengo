cmake_minimum_required(VERSION 3.10)

# Enable testing
enable_testing()

# Add test executables
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic nengo_cpp)

add_test(NAME basic_tests COMMAND test_basic)

add_executable(test_network test_network.cpp)
target_link_libraries(test_network nengo_cpp)

add_test(NAME network_tests COMMAND test_network)