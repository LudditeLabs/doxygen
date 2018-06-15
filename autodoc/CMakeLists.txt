# configvalues.h
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -maph ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configvalues.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.h
)
set_source_files_properties(${GENERATED_SRC}/configvalues.h PROPERTIES GENERATED 1)

include_directories(
    ${CMAKE_SOURCE_DIR}/qtools
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
    ${CLANG_INCLUDEDIR}
    ${GENERATED_SRC}
    ${PYTHON_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${PYTHON_LIBRARIES})

add_executable(autodocdb autodocdb.cpp)

target_link_libraries(autodocdb
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

install(TARGETS autodocdb DESTINATION bin)

if (build_tests)
    add_subdirectory(tests)
endif()
