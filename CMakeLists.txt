# CMakeLists.txt for PspmtAnalysis package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(merger)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(PAASS_DIR_PATH "/home/ryokoyam/vandle/trace_yso/paass/install" CACHE STRING "path to paass directory" )
set(YAML_CPP_PATH "/home/ryokoyam/opt/yaml-cpp" CACHE STRING "path to yaml-cpp")
set( YAML_CPP_LIB_DIR ${YAML_CPP_PATH}/lib/ CACHE STRING "path to yaml lib")
set( MERGER_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib/ CACHE STRING "path to install lib")
configure_file( anamerger/include/LibraryConfig.h.in ${CMAKE_BINARY_DIR}/generated/LibraryConfig.h)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net Proof)
#find_package(yaml-cpp)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/anamerger/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tree_dump2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/trace_analyzer)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tree_processors/include)
include_directories(${YAML_CPP_PATH}/include)
include_directories(${PAASS_DIR_PATH}/include)
include_directories(${CMAKE_BINARY_DIR}/generated)
link_directories(${PAASS_DIR_PATH}/lib)
link_directories(${YAML_CPP_PATH}/lib)
link_directories(${PWD}/anaerger/source)
link_directories(${PWD}/tree_dump)
link_directories(${PWD}/trace_analyzer)
link_directories(${ROOTSYS}/lib)
ROOT_GENERATE_DICTIONARY(G__MergerData include/BigRIPSTreeData.h include/OutputTreeData.hpp 
        trace_analyzer/PspmtAnalyzerData.hpp trace_analyzer/PspmtData.hpp LINKDEF include/TreeDataLinkDef.h)

#---Create a shared library with geneated dictionary
add_library(merger_data_dic SHARED G__MergerData.cxx)

#---Create  a main program using the library
add_subdirectory(source)
add_subdirectory(anamerger)
add_subdirectory(trace_analyzer)
add_subdirectory(share)
#add_subdirectory(tree_processors)
add_executable(merger_main MergerMain.cpp)
add_executable(merger_pid MergerImpPid.cpp)
add_executable(merger_beta MergerImpBeta.cpp)
#add_executable(merger_imp_beta MergerBetaImp.cpp)
add_executable(anamerger_main anamerger/source/AnamergerMain.cpp)
add_executable(trace_analyzer_main trace_analyzer/TraceAnalyzerMain.cpp)

target_link_libraries(merger_main ${ROOT_LIBRARIES} yaml-cpp MergerLib
   TraceAnalyzerLib merger_data_dic)
target_link_libraries(merger_pid ${ROOT_LIBRARIES} yaml-cpp MergerLib
   TraceAnalyzerLib merger_data_dic)
target_link_libraries(merger_beta ${ROOT_LIBRARIES} yaml-cpp MergerLib
   TraceAnalyzerLib merger_data_dic)
#target_link_libraries(merger_imp_beta ${ROOT_LIBRARIES} yaml-cpp MergerData
#   MergerLib tree_dump_dic)
target_link_libraries(anamerger_main ${ROOT_LIBRARIES} yaml-cpp MergerLib merger_data_dic
   SysRootStrucLib TraceAnalyzerLib AnamergerLib )
target_link_libraries(trace_analyzer_main ${ROOT_LIBRARIES} yaml-cpp TraceAnalyzerLib SysRootStrucLib MergerLib merger_data_dic)

install(TARGETS trace_analyzer_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS anamerger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS merger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS merger_pid
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS merger_beta
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS merger_data_dic
	LIBRARY DESTINATION lib)
install(FILES build/libMergerData_rdict.pcm DESTINATION lib)
