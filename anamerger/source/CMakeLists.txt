include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__Anamerger ../include/AnamergerSelector.h LINKDEF ../include/AnamergerLinkDef.h)
ROOT_GENERATE_DICTIONARY(G__Correction ../include/CorrectionSelector.h ../include/CorrectedVANDLEData.h LINKDEF ../include/CorrectionLinkDef.h)

add_library(AnamergerLib SHARED
  G__Anamerger.cxx
  AnamergerSelector.cpp
)

add_library(CorrectionLib SHARED
  G__Correction.cxx
  CorrectionSelector.cpp
  VANDLEToFCorrector.cpp
)

install(TARGETS AnamergerLib LIBRARY DESTINATION lib)
install(TARGETS CorrectionLib LIBRARY DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libAnamerger_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libCorrection_rdict.pcm DESTINATION lib)
