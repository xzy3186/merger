#ifndef PSPMT__DATA_HPP
#define PSPMT__DATA_HPP

#include "ProcessorRootStruc.hpp"

struct pspmt_struc{
   Double_t pos_x_ = -999;
   Double_t pos_y_ = -999;
   Int_t valid_ = 0;
   Double_t trace_energy_ = 0;
};
static pspmt_struc DEFAULT_PSPMT_STRUC;

class PspmtData : public TObject {
public:

   PspmtData(){}
   virtual ~PspmtData(){}

   PspmtData(const PspmtData &obj):TObject(obj)
   {
      external_ts_high_ = obj.external_ts_high_;
      external_ts_low_ = obj.external_ts_low_;
      high_gain_ = obj.high_gain_;
      low_gain_ = obj.low_gain_;
   }

   void Clear(){
      external_ts_high_ = 0;
      external_ts_low_ = 0;
      high_gain_ = DEFAULT_PSPMT_STRUC;
      low_gain_ = DEFAULT_PSPMT_STRUC;
   }

   ULong64_t external_ts_high_;
   ULong64_t external_ts_low_;
   pspmt_struc high_gain_;
   pspmt_struc low_gain_;
  
   ClassDef(PspmtData,1)
};


#endif
