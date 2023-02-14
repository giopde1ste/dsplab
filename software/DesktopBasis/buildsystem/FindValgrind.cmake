# Copyright (C) 2007  University of Rostock
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA.

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(VALGRIND_EXECUTABLE
  NAMES valgrind
  DOC "Valgrind is an instrumentation framework for building dynamic analysis tools. See http://valgrind.org/ for details."
  PATHS ${CMAKE_SOURCE_DIR}/bin
 )

if (VALGRIND_EXECUTABLE)
set(VALGRIND_FOUND true)
endif()

MARK_AS_ADVANCED(
   VALGRIND_FOUND
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Valgrind "Could NOT find valgrind tool! See http://valgrind.org/ for details." VALGRIND_EXECUTABLE)
