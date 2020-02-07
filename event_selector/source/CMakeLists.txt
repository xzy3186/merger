ROOT_GENERATE_DICTIONARY(G__TBetaEventList
../include/TBetaEventList.h ../include/BetaEventSelector.h LINKDEF ../include/BetaEventListLinkDef.h)

add_library(BetaEventListLib SHARED
	G__TBetaEventList.cxx
	TBetaEventList.cpp
	BetaEventSelector.cpp
)
target_link_libraries(BetaEventListLib ${ROOT_LIBRARIES})
install(TARGETS BetaEventListLib LIBRARY DESTINATION lib)

link_directories(${PWD})
add_executable(event_list BetaEventListMain.cpp)
target_link_libraries(event_list BetaEventListLib TraceAnalyzerLib CorrectionLib PaassRootStruct MergerLib ${YAML_CPP_LIBRARIES} ${ROOT_LIBRARIES})

install(TARGETS event_list RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libTBetaEventList_rdict.pcm DESTINATION lib)
