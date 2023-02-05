#
# Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/buffers
#

cmake_minimum_required(VERSION 3.8...3.16)

project(boost_buffers VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

file(GLOB_RECURSE BOOST_BUFFERS_HEADERS CONFIGURE_DEPENDS
    include/boost/*.hpp
    include/boost/*.ipp
    include/boost/*.natvis
)
set(BOOST_BUFFERS_SOURCES src/src.cpp)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/boost PREFIX "" FILES ${BOOST_BUFFERS_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "" FILES ${BOOST_BUFFERS_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES Jamfile)

#include(GNUInstallDirs)
#set(BOOST_INCLUDE_LIBRARIES buffers)
#set(BOOST_EXCLUDE_LIBRARIES buffers)
#set(CMAKE_FOLDER Dependencies)
#add_subdirectory(../.. Dependencies/boost EXCLUDE_FROM_ALL)
#unset(CMAKE_FOLDER)
#set_property(TARGET boost_assert PROPERTY FOLDER "Dep")
#set_property(TARGET Boost::config PROPERTY FOLDER "Dep")
#set_property(TARGET boost_core PROPERTY FOLDER "Dep")
#set_property(TARGET Boost::static_assert PROPERTY FOLDER "Dep")
#set_property(TARGET Boost::system PROPERTY FOLDER "Dep")
#set_property(TARGET Boost::throw_exception PROPERTY FOLDER "Dep")
#set_property(TARGET boost_variant2 PROPERTY FOLDER "Dep")
#set(CMAKE_FOLDER "_Dep")
#add_subdirectory(libs/ _Dep/boost EXCLUDE_FROM_ALL)
#unset(CMAKE_FOLDER)

add_library(boost_buffers ${BOOST_BUFFERS_HEADERS} ${BOOST_BUFFERS_SOURCES})
add_library(Boost::buffers ALIAS boost_buffers)

target_include_directories(
    boost_buffers PUBLIC "include")

target_link_libraries(boost_buffers
    PUBLIC
        Boost::assert
        Boost::config
        Boost::static_assert
        Boost::system
        Boost::throw_exception
    PRIVATE
        Boost::core
)

target_compile_features(boost_buffers PUBLIC cxx_std_11)

target_compile_definitions(boost_buffers
    PUBLIC BOOST_BUFFERS_NO_LIB)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(boost_buffers PUBLIC BOOST_BUFFERS_DYN_LINK)
else()
    target_compile_definitions(boost_buffers PUBLIC BOOST_BUFFERS_STATIC_LINK)
endif()

#if(BUILD_TESTING)
  add_subdirectory(test)
#endif()
