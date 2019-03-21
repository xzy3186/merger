
#ROOT_GENERATE_DICTIONARY(G__TreeData ../include/BigRIPSTreeData.h
#  OutputTreeData.hpp LINKDEF ../include/TreeDataLinkDef.h)

add_library(MergerLib SHARED
  RemainTime.cpp
  YamlParameter.cpp
  YamlReader.cpp
  YSOMap.cxx
  BetaTSScannor.cpp
  BigRIPSTSScannor.cpp
  ImplantTSScannor.cpp
#  G__TreeData.cxx
)

#target_link_libraries(MergerLib SysRootStrucLib)
target_link_libraries(MergerLib merger_data_dic SysRootStrucLib ${ROOT_LIBRARIES})

install(TARGETS MergerLib DESTINATION lib)
