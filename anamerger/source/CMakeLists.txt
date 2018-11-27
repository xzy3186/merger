include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__TreeDataA ../../include/BigRIPSTreeData.h ../include/AnamergerPidSelector.h ../include/AnamergerVANDLESelector.h LINKDEF ../../include/TreeDataLinkDef.h)

add_library(AnamergerLib SHARED
  G__TreeDataA.cxx
  AnamergerPidSelector.cxx
  AnamergerVANDLESelector.cxx
  hIsotope.cxx
)

target_link_libraries(AnamergerLib Core Imt RIO Net Hist Graf Graf3d Gpad ROOTDataFrame ROOTVecOps Tree TreePlayer Rint Postscript Matrix Physics MathCore Thread MultiProc Proof Cling setDict SysRootStrucLib)
