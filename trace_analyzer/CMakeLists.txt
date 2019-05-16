include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__TraceAnalyzerData TraceAnalyzerData.hpp PspmtAnalyzerData.hpp PspmtData.hpp LINKDEF TraceAnalyzerDataLinkDef.hpp)
add_library(TraceAnalyzerLib SHARED
PspmtAnalyzer.cpp
#TraceAnalyzer.cpp
G__TraceAnalyzerData.cxx
)

install(TARGETS TraceAnalyzerLib LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libTraceAnalyzerData_rdict.pcm DESTINATION lib)
