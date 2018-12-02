include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__TreeDumpData DumpTreeData.h LINKDEF TreeDumpDataLinkDef.h)
add_library(tree_dump_dic SHARED G__TreeDumpData.cxx)
add_executable(treedump TreeDump.cpp)

target_link_libraries(treedump tree_dump_dic ${ROOT_LIBRARIES} )

#target_link_libraries(treedump tree_dump_dic Core Imt RIO Net Hist Graf Graf3d Gpad
#   ROOTDataFrame ROOTVecOps Tree TreePlayer Rint Postscript Matrix Physics
#   MathCore Thread MultiProc Proof Cling setDict SysRootStrucLib )
