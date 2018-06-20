project(autodoc_tests)

function(fix_visibility)
    foreach(item ${ARGV})
        message(STATUS "Update visibility for ${item}")
        set_target_properties(${item} PROPERTIES CXX_VISIBILITY_PRESET "default")
        set_target_properties(${item} PROPERTIES C_VISIBILITY_PRESET "default")
        set_target_properties(${item} PROPERTIES VISIBILITY_INLINES_HIDDEN 0)
    endforeach()
endfunction()

# http://www.kaizou.org/2014/11/gtest-cmake/
# https://crascit.com/2015/07/25/cmake-gtest/

# Download and unpack googletest at configure time
configure_file(CMakeLists-gtest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/googletest-download" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/googletest-download" )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build")

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
endif()


set(TEST_PYTHON_MODULES_PATH ${EXECUTABLE_OUTPUT_PATH})
set(TEST_PYTHON_FILES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pytests)
configure_file(testconfig.h.in testconfig.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SRC LIST_DIRECTORIES false "*.cpp")
file(GLOB PY_SRC LIST_DIRECTORIES false "pytests/*.py")
add_executable(autodoctests ${SRC} ${PY_SRC})
target_link_libraries(autodoctests
    _doxygen
    qtools
    md5
    doxycfg
    vhdlparser
    ${ICONV_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SQLITE3_LIBRARIES}
    ${EXTRA_LIBS}
    ${CLANG_LIBS}
    ${PYTHON_LIBRARY}
    gtest gmock
)

add_test(NAME autodoctests COMMAND autodoctests WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
fix_visibility(gtest gmock gtest_main gmock_main)

add_subdirectory(pymodule)
