# CMakeLists.txt for PspmtAnalysis package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(merger)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(PAASS_DIR_PATH "/home/vandle18/opt/paass/install" CACHE STRING "path to paass directory" )
set(YAML_CPP_PATH "/home/vandle18/opt/yaml-cpp" CACHE STRING "path to yaml-cpp")

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)
#find_package(yaml-cpp)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${YAML_CPP_PATH}/include)
include_directories(${PAASS_DIR_PATH}/include)
link_directories(${PAASS_DIR_PATH}/lib)
link_directories(${YAML_CPP_PATH}/lib)
ROOT_GENERATE_DICTIONARY(G__MergerData include/BigRIPSTreeData.h include/OutputTreeData.hpp LINKDEF include/TreeDataLinkDef.h)

#---Create a shared library with geneated dictionary
add_library(MergerData SHARED G__MergerData.cxx)
target_link_libraries(MergerData ${ROOT_LIBRARIES})
#target_link_libraries(PspmtPositionData ${ROOT_LIBRARIES} yaml-cpp)

#---Create  a main program using the library
add_subdirectory(source)
add_executable(merger MergerMain.cpp)
target_link_libraries(merger ${ROOT_LIBRARIES} yaml-cpp MergerData MergerLib)
#target_link_libraries(pspmtana PspmtAnalyzer ${ROOT_LIBRARIES} PspmtPositionData yaml-cpp)

