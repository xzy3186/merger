# CMakeLists.txt for PspmtAnalysis package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(merger)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/ CACHE PATH "install dir" FORCE)
set(PAASS_DIR_PATH $ENV{PAASSSYS} CACHE STRING "path to paass directory" )
set( MERGER_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib/ CACHE STRING "path to install lib")
set(MERGER_WORK_DIR $ENV{HOME}/merger_work/ CACHE STRING "path to working directory")
set(HOME $ENV{HOME} )
option(BUILD_ANAMERGER "Build and install anamerger" OFF)
option(BUILD_EVENTSELECTOR "Build and install event_selector" OFF)
configure_file( anamerger/include/LibraryConfig.h.in ${CMAKE_BINARY_DIR}/generated/LibraryConfig.h)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net Proof)
find_package(yaml-cpp)
set(YAML_CPP_LIB_DIR ${YAML_CPP_CMAKE_DIR}/../../../lib CACHE PATH "yaml-cpp lib dir")

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(BUILD_ANAMERGER)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/anamerger/include)
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tree_dump2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/trace_analyzer)
if(BUILD_EVENTSELECTOR)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/event_selector/include)
endif()
include_directories(${YAML_CPP_INCLUDE_DIR})
include_directories(${PAASS_DIR_PATH}/include)
include_directories(${CMAKE_BINARY_DIR}/generated)
link_directories(${PAASS_DIR_PATH}/lib)
link_directories(${YAML_CPP_LIB_DIR})
if(BUILD_ANAMERGER)
    link_directories(${PWD}/anamerger/source)
endif()
link_directories(${PWD}/tree_dump)
link_directories(${PWD}/trace_analyzer)
if(BUILD_EVENTSELECTOR)
    link_directories(${PWD}/event_selector/source)
endif()
link_directories(${ROOTSYS}/lib)
ROOT_GENERATE_DICTIONARY(G__MergerData include/BigRIPSTreeData.h include/OutputTreeData.hpp include/E19044Data.hpp include/FDSiData.hpp
        trace_analyzer/PspmtAnalyzerData.hpp trace_analyzer/PspmtData.hpp LINKDEF include/TreeDataLinkDef.h)

#---Create a shared library with geneated dictionary
add_library(merger_data_dic SHARED G__MergerData.cxx)

#---Create  a main program using the library
add_subdirectory(source)
if(BUILD_ANAMERGER)
    add_subdirectory(anamerger)
endif()
add_subdirectory(trace_analyzer)
add_subdirectory(share)
if(BUILD_EVENTSELECTOR)
    add_subdirectory(event_selector)
endif()
add_subdirectory(scripts)
add_subdirectory(config)
add_subdirectory(mergerLists)
add_subdirectory(anamergerLists)
add_executable(merger_main MergerMain.cpp)
add_executable(next_merger_main NextMergerMain.cpp)
add_executable(fdsi_test_merger_main FDSiTestMergerMain.cpp)
if(BUILD_ANAMERGER)
    add_executable(anamerger_main anamerger/source/AnamergerMain.cpp)
    add_executable(anamerger_sparse_main anamerger/source/AnamergerSparseMain.cpp)
    #add_executable(correction_main anamerger/source/CorrectionMain.cpp)
endif()
add_executable(trace_analyzer_main trace_analyzer/TraceAnalyzerMain.cpp)

target_link_libraries(merger_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib
   TraceAnalyzerLib merger_data_dic)
target_link_libraries(next_merger_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib
   TraceAnalyzerLib merger_data_dic)
target_link_libraries(fdsi_test_merger_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib
   TraceAnalyzerLib merger_data_dic)
if(BUILD_ANAMERGER)
    target_link_libraries(correction_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib merger_data_dic
       PaassRootStruct TraceAnalyzerLib AnamergerLib CorrectionLib )
    target_link_libraries(anamerger_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib merger_data_dic
       PaassRootStruct TraceAnalyzerLib AnamergerLib CorrectionLib )
    target_link_libraries(anamerger_sparse_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} MergerLib merger_data_dic
       PaassRootStruct TraceAnalyzerLib AnamergerLib CorrectionLib )
endif()
target_link_libraries(trace_analyzer_main ${ROOT_LIBRARIES} ${YAML_CPP_LIBRARIES} TraceAnalyzerLib PaassRootStruct MergerLib merger_data_dic)

install(TARGETS trace_analyzer_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
if(BUILD_ANAMERGER)
install(TARGETS anamerger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS anamerger_sparse_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS correction_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
endif()
install(TARGETS merger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS next_merger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS fdsi_test_merger_main
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(TARGETS merger_data_dic
	LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libMergerData_rdict.pcm DESTINATION lib)
