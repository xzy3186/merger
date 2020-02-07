#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


#pragma link C++ class std::vector<ULong64_t>+;
#pragma link C++ class std::map<std::string,std::vector<ULong64_t>>+;
#pragma link C++ class TBetaEventList+;
#pragma link C++ class BetaEventSelector+;
#pragma link C++ class TTreeReaderValue<OutputTreeData<PspmtData, OutputTreeData<PspmtData,TreeData>>>+;
#pragma link C++ class TTreeReaderValue<std::vector<processor_struct::CLOVERS>>+;
#pragma link C++ class TTreeReaderValue<std::vector<processor_struct::VANDLES>>+;
#pragma link C++ class TTreeReaderValue<std::vector<processor_struct::GAMMASCINT>>+;

#endif

