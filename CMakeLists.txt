cmake_minimum_required(VERSION 3.5)

project(counterfeit_crc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG        v3.16.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(counterfeit_crc
    main.cpp
    crc.h
    crc_impl.h
    crc_file_utils.h
    )

target_link_libraries(counterfeit_crc PRIVATE Catch2::Catch2WithMain)
