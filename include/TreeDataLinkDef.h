#ifdef __CINT__

//#pragma link off all globals;
//#pragma link off all classes;
//#pragma link off all functions;

//#pragma link C++ class processor_struct::CLOVERS+;
//#pragma link C++ class std::vector<processor_struct::CLOVERS>+;
//#pragma link C++ class processor_struct::DOUBLEBETA+;
//#pragma link C++ class std::vector<processor_struct::DOUBLEBETA>+;
//#pragma link C++ class processor_struct::GAMMASCINT+;
//#pragma link C++ class std::vector<processor_struct::GAMMASCINT>+;
//#pragma link C++ class processor_struct::LOGIC+;
//#pragma link C++ class std::vector<processor_struct::LOGIC>+;
//#pragma link C++ class processor_struct::LOGIC+;
//#pragma link C++ class std::vector<processor_struct::LOGIC>+;

//#pragma link C++ class PixTreeEvent+;

#pragma link C++ class TreeData+;
#pragma link C++ class OutputTreeData<PspmtData, TreeData>+;
#pragma link C++ class OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, TreeData>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>>+;
#pragma link C++ class AnamergerPidSelector+;
#pragma link C++ class AnamergerVANDLESelector+;
#pragma link C++ class OutputTreeData<DumpTreeData, TreeData>+;
#pragma link C++ class OutputTreeData<DumpTreeData, OutputTreeData<DumpTreeData,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<DumpTreeData,TreeData>, DumpTreeData>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<DumpTreeData, TreeData>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<DumpTreeData, OutputTreeData<DumpTreeData,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<DumpTreeData,TreeData>, DumpTreeData>>+;

#endif

