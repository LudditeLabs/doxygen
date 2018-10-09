# Copyright (C) 2018, Luddite Labs Inc.
#
# This file is part of doxygen.
#
# Doxygen is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(autodoc)

include_directories(
    ${CMAKE_SOURCE_DIR}/qtools
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
    ${CLANG_INCLUDEDIR}
    ${GENERATED_SRC}
    ${PYTHON_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${PYTHON_LIBRARIES})

add_executable(contentdb optparse/optparse.h main_contentdb.cpp)

target_link_libraries(contentdb
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

install(TARGETS contentdb DESTINATION bin)

if (build_tests)
    add_subdirectory(tests)
endif()
