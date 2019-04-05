#ifdef __CINT__

//#pragma link off all globals;
//#pragma link off all classes;
//#pragma link off all functions;

#pragma link C++ class TreeData+;
#pragma link C++ class OutputTreeData<PspmtData, TreeData>+;
#pragma link C++ class OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>+;
//#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, TreeData>>+;
//#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>>+;
//#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>>+;

#endif

