cmake_minimum_required(VERSION 3.0)
project(spiking_slam LANGUAGES CXX)
# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)
# need this to include the auto generated ui_mainwindow.h
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Add the local "cmake" directory to the search path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Enable C++14 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all warnings on MSVC and GCC/Clang/Intel
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
                string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()
elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR
                ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR
                ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Set a default build type
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, optios are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# Add configuration option for static linking
set(STATIC_LINKING FALSE CACHE BOOL "True for static linking")

# Link statically
if(STATIC_LINKING)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        SET(BUILD_SHARED_LIBRARIES OFF)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -Wl,--no-export-dynamic")
        SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Wl,--no-export-dynamic")
    else()
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
        SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
    endif()
endif()

# Include the Cypress library
include(cypress_external)

add_library(spiking_part
    source/simulation
    source/spikingnetwork)
target_link_libraries(spiking_part
	${CYPRESS_LIBRARY})
add_dependencies(spiking_part cypress_ext)


include_directories(source)
set(project_sources source/animation.cpp source/dialog.cpp source/mainwindow.cpp source/gui.cpp)
set(project_headers source/animation.h source/dialog.h source/mainwindow.h)
set(project_ui source/dialog.ui source/mainwindow.ui)
# wrap the ui file to a c++ header
qt5_wrap_ui(ui_wrap ${project_ui})
qt5_wrap_cpp(moc_sources ${project_headers})
add_executable(gui  
	${project_sources} 
	${project_headers}
	${ui_wrap} 
	${moc_sources})
add_dependencies(gui cypress_ext)
target_link_libraries(gui
	PUBLIC 
	spiking_part
	Qt5::Core
	Qt5::Gui 
	Qt5::Widgets)

add_executable(main source/main.cpp)
add_dependencies(main cypress_ext)
target_link_libraries(main
	PUBLIC
	spiking_part)
