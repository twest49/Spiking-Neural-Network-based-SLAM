#  SNABSuite -- Spiking Neural Architecture Benchmark Suite
#  Copyright (C) 2018  Christoph Jenzen
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.


find_package(PythonLibs 3 REQUIRED )
find_package(PythonInterp 3 REQUIRED)

include(ExternalProject)


# Add configuration option for static linking
set(UPDATE_CYPRESS FALSE CACHE BOOL "True for update cypress every time")
if(UPDATE_CYPRESS)
    ExternalProject_Add(cypress_ext
        GIT_REPOSITORY        "https://github.com/hbp-unibi/cypress/"
        GIT_TAG               master
        CMAKE_ARGS            -DSTATIC_LINKING=${STATIC_LINKING} -DCMAKE_INSTALL_PREFIX:path=<INSTALL_DIR> -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DBUILD_TEST_EXAMPLES=False -DPYTHON_VERSION_MAJOR=${PYTHON_VERSION_MAJOR}
        INSTALL_COMMAND 	  ""
        UPDATE_COMMAND git pull
        EXCLUDE_FROM_ALL      TRUE
        BUILD_ALWAYS    FALSE
    )
else()
    ExternalProject_Add(cypress_ext
        GIT_REPOSITORY        "https://github.com/hbp-unibi/cypress/"
        GIT_TAG               master
        CMAKE_ARGS            -DSTATIC_LINKING=${STATIC_LINKING} -DCMAKE_INSTALL_PREFIX:path=<INSTALL_DIR> -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DBUILD_TEST_EXAMPLES=False -DPYTHON_VERSION_MAJOR=${PYTHON_VERSION_MAJOR}
        INSTALL_COMMAND 	  ""
        UPDATE_COMMAND ""
        EXCLUDE_FROM_ALL      TRUE
        BUILD_ALWAYS    FALSE
    )
endif()
ExternalProject_Get_Property(cypress_ext SOURCE_DIR BINARY_DIR)


set(CYPRESS_INCLUDE_DIRS 
	${SOURCE_DIR}/
	${SOURCE_DIR}/external/pybind11/include/
	${BINARY_DIR}/include/
	${PYTHON_INCLUDE_DIRS}
)
set(CYPRESS_LIBRARY
	${BINARY_DIR}/libcypress.a
    ${PYTHON_LIBRARIES}
    -pthread
    dl
)

if(STATIC_LINKING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${BINARY_DIR}/include/glibc.h")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include ${BINARY_DIR}/include/glibc.h")
endif()

set(GTEST_LIBRARIES
    ${BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest.a
	${BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest_main.a
)
set(GTEST_INCLUDE_DIRS ${BINARY_DIR}/googletest-prefix/src/googletest/googletest/include/)
set(GTEST_FOUND TRUE)

execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "from __future__ import print_function\nimport numpy; print(numpy.get_include(), end='')"
            OUTPUT_VARIABLE numpy_path)
            
find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h 
    HINTS "${numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)

include_directories(
	${CYPRESS_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
    ${PYTHON_NUMPY_INCLUDE_DIR}
)
