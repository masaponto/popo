cmake_minimum_required(VERSION 2.8)

PROJECT(popo_test)

include_directories(
    ${PROJECT_SOURCE_DIR}/gtest-1.7.0/include
    ${PROJECT_SOURCE_DIR}/..
    #    /home/hima/work/popo/src/test/gtest-1.7.0/include
    )

add_definitions(
    -std=c++11
    -ggdb3
    )

add_executable(
    popo_test
    popo_test.cpp
    )

#add_executable(lexical_test lexical_test.cpp)
#add_executable(syntax_test syntax_test.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/gtest-1.7.0)

target_link_libraries(
    popo_test
    pthread
    gtest
    ${PROJECT_SOURCE_DIR}/gtest-1.7.0/build/libgtest.a
    ${PROJECT_SOURCE_DIR}/gtest-1.7.0/build/libgtest_main.a
    )
