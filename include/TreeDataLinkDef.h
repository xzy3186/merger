#ifdef __CINT__

//#pragma link off all globals;
//#pragma link off all classes;
//#pragma link off all functions;

#pragma link C++ class TreeData+;
#pragma link C++ class PixTreeEvent+;
#pragma link C++ class OutputTreeData<PixTreeEvent, TreeData>+;
#pragma link C++ class OutputTreeData<PixTreeEvent, OutputTreeData<PixTreeEvent,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<PixTreeEvent,TreeData>, PixTreeEvent>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PixTreeEvent, TreeData>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PixTreeEvent, OutputTreeData<PixTreeEvent,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<PixTreeEvent,TreeData>, PixTreeEvent>>+;
#pragma link C++ class AnamergerPidSelector+;
#pragma link C++ class AnamergerVANDLESelector+;
#pragma link C++ class OutputTreeData<DumpTreeData, TreeData>+;
#pragma link C++ class OutputTreeData<DumpTreeData, OutputTreeData<DumpTreeData,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<DumpTreeData,TreeData>, DumpTreeData>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<DumpTreeData, TreeData>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<DumpTreeData, OutputTreeData<DumpTreeData,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<DumpTreeData,TreeData>, DumpTreeData>>+;

#endif

