add_library(visitor MODULE visitormodule.cpp)
target_link_libraries(visitor
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
)
set_target_properties(visitor PROPERTIES PREFIX "")
set_target_properties(visitor PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

fix_visibility(visitor)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Add -fPIC to doxygen libs")
    set_target_properties(_doxygen PROPERTIES COMPILE_FLAGS "-fPIC")
    set_target_properties(qtools PROPERTIES COMPILE_FLAGS "-fPIC")
    set_target_properties(md5 PROPERTIES COMPILE_FLAGS "-fPIC")
    set_target_properties(doxycfg PROPERTIES COMPILE_FLAGS "-fPIC")
    set_target_properties(vhdlparser PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
