#ifndef PSPMT_ANALYZER_HPP
#define PSPMT_ANALYZER_HPP
#include <fstream>
#include <TTree.h>
#include "ProcessorRootStruc.hpp"
#include "PspmtAnalyzerData.hpp"
#include "TraceAnalyzer.hpp"
#include "YamlReader.hpp"

class PspmtAnalyzer {
public:
   const static std::string kMsgPrefix;

   PspmtAnalyzer(){};
   PspmtAnalyzer(const std::string &yaml_node_name){
      Configure(yaml_node_name);
   }
   virtual ~PspmtAnalyzer(){};

   virtual int Configure(const std::string &yaml_node_name);
   virtual int Begin();
   virtual int Process(const std::vector<parameter_struc> &pspmt);
   virtual int Terminate();

   void CalculatePositionH(pspmt_data_struc &data);
   void CalculatePositionL(pspmt_data_struc &data);

protected:
   std::fstream pos;
   TTree *output_tree_;
   PspmtAnalyzerData data_;
   
};

const std::string TraceAnalyzer::kMsgPrefix("[TraceAnalyzer:]");
const std::string PspmtAnalyzer::kMsgPrefix("[PspmtAnalyzer:]");
#endif