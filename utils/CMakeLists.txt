add_library(utils STATIC
        http_get.cpp
        http_get.hpp

        make_endpoint.hpp
        make_endpoint.cpp
        )

target_link_libraries(utils)
target_include_directories(utils PRIVATE ${JSONCPP_INCLUDE_DIRS})

#add_subdirectory(test)
