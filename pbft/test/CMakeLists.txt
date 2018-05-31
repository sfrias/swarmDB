set(test_srcs pbft_test.cpp)
set(test_libs pbft bootstrap)

add_gmock_test(pbft_tests)

