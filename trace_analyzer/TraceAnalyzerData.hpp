#ifndef TRACE_ANALYZER_DATA_HPP
#define TRACE_ANALYZER_DATA_HPP

#include "ProcessorRootStruc.hpp"

class TraceAnalyzerData : public TObject {
public:

   TraceAnalyzerData(){}
   virtual ~TraceAnalyzerData(){}

   TraceAnalyzerData(const TraceAnalyzerData &obj):TObject(obj)
   {
      external_ts_ = obj.external_ts_;
      pspmt_ = obj.pspmt_;
      trace_energy_ = obj.trace_energy_;
      trace_time_ = obj.trace_time_;
   }

   void Clear(){
      external_ts_ = 0;
      pspmt_ = processor_struct::PSPMT_DEFAULT_STRUCT;
      trace_energy_ = -999;
      trace_time_ = -999;
   }

   ULong64_t external_ts_;
   processor_struct::PSPMT pspmt_;
   Double_t trace_energy_;
   Double_t trace_time_;

   ClassDef(TraceAnalyzerData,1)
};


#endif
