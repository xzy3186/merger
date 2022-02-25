
#ROOT_GENERATE_DICTIONARY(G__TreeData ../include/BigRIPSTreeData.h
#  OutputTreeData.hpp LINKDEF ../include/TreeDataLinkDef.h)

add_library(MergerLib SHARED
  RemainTime.cpp
  YamlParameter.cpp
  YamlReader.cpp
  YSOMap.cxx
  #BetaTSScannor.cpp
  BigRIPSTSScannor.cpp
  ImplantTSScannor.cpp
  E19044BetaTSScanor.cpp
  E19044NEXTTSScanor.cpp
  E19044ImplantTSScanor.cpp
  E19044BetaTreeMerger.cpp
  FDSiBetaTSScanor.cpp
  FDSiVANDLETSScanor.cpp
  FDSiImplantTSScanor.cpp
  FDSiBetaTreeMerger.cpp
#  G__TreeData.cxx
)

#target_link_libraries(MergerLib SysRootStrucLib)
target_link_libraries(MergerLib merger_data_dic PaassRootStruct ${ROOT_LIBRARIES})

install(TARGETS MergerLib DESTINATION lib)
