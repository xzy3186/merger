include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__TraceAnalyzerData TraceAnalyzerData.hpp LINKDEF TraceAnalyzerDataLinkDef.hpp)
add_library(TraceAnalyzerLib SHARED
TraceAnalyzer.cpp
G__TraceAnalyzerData.cxx
)

install(TARGETS TraceAnalyzerLib LIBRARY DESTINATION lib)
install(FILES ../build/trace_analyzer/libTraceAnalyzerData_rdict.pcm DESTINATION lib)