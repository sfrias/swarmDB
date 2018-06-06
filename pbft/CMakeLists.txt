add_library(pbft STATIC
        pbft_base.hpp
        pbft.hpp
        pbft.cpp

        pbft_operation.hpp
        pbft_operation.cpp
        )

target_link_libraries(pbft utils proto)
target_include_directories(pbft PRIVATE ${JSONCPP_INCLUDE_DIRS} ${PROTO_INCLUDE_DIR})

add_subdirectory(test)
