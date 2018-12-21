#ifndef PSPMT_ANALYZER_DATA_HPP
#define PSPMT_ANALYZER_DATA_HPP

#include "ProcessorRootStruc.hpp"
#include "TraceAnalyzerData.hpp"

struct pspmt_data_struc{
   TraceAnalyzerData dynode_;
   TraceAnalyzerData xa_;
   TraceAnalyzerData xb_;
   TraceAnalyzerData ya_;
   TraceAnalyzerData yb_;
   Double_t pos_x_ = -999;
   Double_t pos_y_ = -999;
   Int_t valid_ = 0;
};
static pspmt_data_struc DEFAULT_PSPMT_DATA_STRUC;

class PspmtAnalyzerData : public TObject {
public:

   PspmtAnalyzerData(){}
   virtual ~PspmtAnalyzerData(){}

   PspmtAnalyzerData(const PspmtAnalyzerData &obj):TObject(obj)
   {
      external_ts_high_ = obj.external_ts_high_;
      external_ts_low_ = obj.external_ts_low_;
      high_gain_ = obj.high_gain_;
      low_gain_ = obj.low_gain_;
      desi_top_ = obj.desi_top_;
      desi_bottom_ = obj.desi_bottom_;
   }

   void Clear(){
      external_ts_high_ = 0;
      external_ts_low_ = 0;
      high_gain_ = DEFAULT_PSPMT_DATA_STRUC;
      low_gain_ = DEFAULT_PSPMT_DATA_STRUC;
   }

   ULong64_t external_ts_high_;
   ULong64_t external_ts_low_;
   pspmt_data_struc high_gain_;
   pspmt_data_struc low_gain_;
   TraceAnalyzerData desi_top_;
   TraceAnalyzerData desi_bottom_;

   ClassDef(PspmtAnalyzerData,1)
};


#endif
