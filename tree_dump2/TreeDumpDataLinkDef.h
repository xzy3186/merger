#ifdef __CINT__

//#pragma link off all globals;
//#pragma link off all classes;
//#pragma link off all functions;

#pragma link C++ class DumpTreeData+;
#pragma link C++ struct processor_struct::GAMMASCINT+;
#pragma link C++ class std::vector<processor_struct::GAMMASCINT>+;

#pragma link C++ struct processor_struct::VANDLES+;
#pragma link C++ class std::vector<processor_struct::VANDLES>+;

#pragma link C++ struct processor_struct::CLOVERS+;
#pragma link C++ class std::vector<processor_struct::CLOVERS>+;

#pragma link C++ struct processor_struct::PSPMT+;
#pragma link C++ class std::vector<processor_struct::PSPMT>+;

#pragma link C++ class PixTreeEvent+;
#pragma link C++ class std::vector<PixTreeEvent>+;
#pragma link C++ class std::vector<DumpTreeData>+;

#endif

