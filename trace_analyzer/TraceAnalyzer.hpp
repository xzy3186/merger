#ifndef TRACE_ANALYZER_HPP
#define TRACE_ANALYZER_HPP

#include <TTree.h>
#include "ProcessorRootStruc.hpp"
#include "TraceAnalyzerData.hpp"
#include "YamlReader.hpp"

struct parameter_struc{
   TTree *tree_ = nullptr;
   TraceAnalyzerData *data_ = nullptr;
   std::vector<TraceAnalyzerData> data_vec_;
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
   virtual int Process(const processor_struct::PSPMT &pspmt, const ULong64_t &ext);
   virtual int Terminate();
   virtual void ClearVec();

   const std::vector<parameter_struc> &GetChannelVec() const{
      return channel_vec_;
   }

protected:
   std::vector<parameter_struc> channel_vec_;
};

#endif
