project(autodoc_tests)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(simple simple.cpp)
target_link_libraries(simple GTest::GTest GTest::Main)

GTEST_ADD_TESTS(simple "" AUTO)
