###############################################################################
# (c) Copyright 2013 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

# Authors:
#   Marco Clemencic <marco.clemencic@cern.ch>
#   Benedikt Hegner <benedikt.hegner@cern.ch>

cmake_minimum_required(VERSION 2.8)

set( PM4hep_VERSION_MAJOR 0 )
set( PM4hep_VERSION_MINOR 1 )
set( PM4hep_VERSION_PATCH 0 )
set( PM4hep_VERSION "${PM4hep_VERSION_MAJOR}.${PM4hep_VERSION_MINOR}" )

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#--- set the default install dir to be in the source directory

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH
    "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."
    FORCE )
  MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite wi
th -D CMAKE_INSTALL_PREFIX" )
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#--- build target---------------------------------------

include_directories(include)
add_library(HepPluginManager SHARED src/PluginManager.cpp src/capi_pluginservice.cpp)
target_link_libraries(HepPluginManager ${CMAKE_DL_LIBS})

add_executable(listcomponents src/listcomponents.cpp src/PluginManager.cpp)
target_link_libraries(listcomponents ${CMAKE_DL_LIBS})


#--- install target-------------------------------------

install(DIRECTORY include/PM4hep
        DESTINATION include )
install(TARGETS listcomponents HepPluginManager
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib )


#--- test target----------------------------------------

add_library(Component MODULE tests/testMacro.cpp)
target_link_libraries(Component HepPluginManager)

add_executable(testLoading tests/testLoading.cpp)
target_link_libraries(testLoading HepPluginManager)

add_custom_command(OUTPUT test.components
                   COMMAND LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}
                           ${CMAKE_BINARY_DIR}/listcomponents
                           -o test.components libComponent.so
                   DEPENDS Component listcomponents )

add_custom_target(ComponentsList ALL DEPENDS test.components)

install(TARGETS Component testLoading
        DESTINATION test )
install(FILES test.components
        DESTINATION test )


#--- target for Doxygen documentation-------------------

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


#--- optional tests depending on BOOST -----------------

#find_package(Boost COMPONENTS unit_test_framework)
#if(Boost_FOUND)
#  include_directories(${Boost_INCLUDE_DIRS})
#  add_executable(UseCases tests/src/UseCases.cpp)
#  target_link_libraries(UseCases ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} HepPluginManager)
#endif(Boost_FOUND)

#--- targets for CMake infrastructure ------------------

configure_file(PM4hepConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/PM4hepConfigVersion.cmake" @ONLY)
configure_file(PM4hepConfig.cmake.in "${PROJECT_BINARY_DIR}/PM4hepConfig.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/PM4hepConfig.cmake"
  "${PROJECT_BINARY_DIR}/PM4hepConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake")
