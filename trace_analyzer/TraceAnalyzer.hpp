#ifndef TRACE_ANALYZER_HPP
#define TRACE_ANALYZER_HPP

#include <TTree.h>
#include "ProcessorRootStruc.hpp"
#include "TraceAnalyzerData.hpp"
#include "YamlReader.hpp"

struct parameter_struc{
   TTree *tree_ = nullptr;
   TraceAnalyzerData *data_ = nullptr;
   std::string subtype_ = "";
   std::string tag_ = "";
};

class TraceAnalyzer {
public:
   const static std::string kMsgPrefix;

   TraceAnalyzer(){};
   TraceAnalyzer(const std::string &yaml_node_name){
      Configure(yaml_node_name);
   }
   virtual ~TraceAnalyzer(){};

   virtual int Configure(const std::string &yaml_node_name);
   virtual int Begin();
   virtual int Process(const processor_struct::PSPMT &pspmt);
   virtual int Terminate();

protected:
   std::vector<parameter_struc> channel_vec_;
};

const std::string TraceAnalyzer::kMsgPrefix("[TraceAnalyzer:]");
#endif
