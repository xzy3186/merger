#ifndef PSPMT_ANALYZER_DATA_HPP
#define PSPMT_ANALYZER_DATA_HPP

#include "PaassRootStruct.hpp"
#include "TraceAnalyzerData.hpp"

struct pspmt_data_struc{
   TraceAnalyzerData dynode_;
   TraceAnalyzerData xa_;
   TraceAnalyzerData xb_;
   TraceAnalyzerData ya_;
   TraceAnalyzerData yb_;
   Double_t pos_x_ = -999;
   Double_t pos_max_x_ = -999;
   Double_t pos_y_ = -999;
   Double_t pos_max_y_ = -999;
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
      rit_b1_ = obj.rit_b1_;
      rit_b2_ = obj.rit_b2_;
      fit_b1_ = obj.fit_b1_;
      fit_b2_ = obj.fit_b2_;
   }

   void Clear(){
      external_ts_high_ = 0;
      external_ts_low_ = 0;
      high_gain_ = DEFAULT_PSPMT_DATA_STRUC;
      low_gain_ = DEFAULT_PSPMT_DATA_STRUC;
		rit_b1_.Clear();
		rit_b2_.Clear();
		fit_b1_.Clear();
		fit_b2_.Clear();
   }

   ULong64_t external_ts_high_;
   ULong64_t external_ts_low_;
   pspmt_data_struc high_gain_;
   pspmt_data_struc low_gain_;
   TraceAnalyzerData rit_b1_;
   TraceAnalyzerData rit_b2_;
   TraceAnalyzerData fit_b1_;
   TraceAnalyzerData fit_b2_;
   ClassDef(PspmtAnalyzerData,2)
};


#endif
