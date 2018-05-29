add_library(pbft STATIC
    pbft_base.hpp
    pbft.hpp
    pbft.cpp

    pbft_networking_base.hpp
    pbft_networking.hpp
    pbft_networking.cpp
  )

target_link_libraries(pbft utils)
target_include_directories(pbft PRIVATE ${JSONCPP_INCLUDE_DIRS})
