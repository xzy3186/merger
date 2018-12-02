
ROOT_GENERATE_DICTIONARY(G__TreeData ../include/BigRIPSTreeData.h
   ../tree_dump2/DumpTreeData.h OutputTreeData.hpp LINKDEF ../include/TreeDataLinkDef.h)

add_library(MergerLib SHARED
  RemainTime.cpp
  YamlParameter.cpp
  YamlReader.cpp
  BetaTSScannor.cpp
  BigRIPSTSScannor.cpp
  ImplantTSScannor.cpp
  G__TreeData.cxx
)

target_link_libraries(MergerLib SysRootStrucLib)
