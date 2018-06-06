set(test_srcs pbft_test.cpp)
set(test_libs pbft protobuf bootstrap)

add_gmock_test(pbft_tests)

