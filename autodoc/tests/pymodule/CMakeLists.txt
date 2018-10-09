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
