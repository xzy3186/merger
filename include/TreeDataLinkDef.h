#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TreeData+;
#pragma link C++ class std::vector<TreeData>+;
#pragma link C++ class OutputTreeData<PspmtData, TreeData>+;
#pragma link C++ class std::vector<OutputTreeData<PspmtData, TreeData>>+;
#pragma link C++ class OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>+;
#pragma link C++ class OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, TreeData>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<OutputTreeData<PspmtData,TreeData>, PspmtData>>+;

#pragma link C++ class E19044Output+;
#pragma link C++ class std::vector<E19044Output>+;
#pragma link C++ class NEXTOutput+;
#pragma link C++ class std::vector<NEXTOutput>+;

#endif

