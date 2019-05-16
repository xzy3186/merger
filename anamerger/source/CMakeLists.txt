include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__Anamerger ../include/AnamergerSelector.h LINKDEF ../include/AnamergerLinkDef.h)

add_library(AnamergerLib SHARED
  G__Anamerger.cxx
  AnamergerSelector.cpp
)

install(TARGETS AnamergerLib LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libAnamerger_rdict.pcm DESTINATION lib)
