#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class vector<processor_struct::CLOVERS>+;
#pragma link C++ class vector<processor_struct::VANDLES>+;
#pragma link C++ class vector<TreeData>+;
#pragma link C++ class pspmt_data_struc+;
#pragma link C++ class PspmtAnalyzerData+;
#pragma link C++ class vector<PspmtAnalyzerData>+;
#pragma link C++ class event_info+;
#pragma link C++ class vector<PspmtData>+;
#pragma link C++ class vector<OutputTreeData<PspmtData,TreeData>>+:
#pragma link C++ class CorrectionSelector+;
#pragma link C++ class CorrectedVANDLEData+;
#pragma link C++ class vector<CorrectedVANDLEData>+;

#endif

