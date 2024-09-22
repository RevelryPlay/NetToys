cmake_minimum_required(VERSION 3.29)
project(Net_Toys)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
        Server/Server.cpp Server/Server.hpp
        Client/Client.cpp Client/Client.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

# Set cmake output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(ArgoDraft STATIC IMPORTED)
#if (WIN32)
#    set_property(TARGET ArgoDraft PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/ArgoDraft.lib)
#endif ()
#
#if (UNIX)
    set_property(TARGET ArgoDraft PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libArgoDraft.a)
#endif ()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC wsock32 ws2_32)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ArgoDraft)