#ifndef PSPMT__DATA_HPP
#define PSPMT__DATA_HPP

#include "PaassRootStruct.hpp"

struct pspmt_struc{
   Double_t pos_x_ = -999;
   Double_t pos_y_ = -999;
   Int_t id_x_ = -999;
   Int_t id_y_ = -999;
   Int_t valid_ = 0;
   Double_t qdc_ = 0;
   Double_t energy_ = 0;
   Double_t time_ = 0;
   Double_t energy_sum_ = 0;

	Double_t xa_energy_ = -999;
	Double_t xb_energy_ = -999;
	Double_t ya_energy_ = -999;
	Double_t yb_energy_ = -999;
	Double_t xa_qdc_ = -999;
	Double_t xb_qdc_ = -999;
	Double_t ya_qdc_ = -999;
	Double_t yb_qdc_ = -999;
};
static pspmt_struc DEFAULT_PSPMT_STRUC;

struct beamline_detector_struc{
   Double_t energy_ = 0;
   Double_t time_ = -9999;
};
static beamline_detector_struc DEFAULT_BEAMLINE_DETECTOR_STRUC;

struct event_info_struc {
   std::string file_name_;
   ULong64_t pixie_event_num_ = 0;
};

class PspmtData : public TObject {
public:

	PspmtData() :TObject() { Clear(); }
   virtual ~PspmtData(){}

   PspmtData(const PspmtData &obj):TObject(obj)
   {
		event_number_ = obj.event_number_;
		file_name_ = obj.file_name_;
      external_ts_high_ = obj.external_ts_high_;
      external_ts_low_ = obj.external_ts_low_;
      rit_b1_ = obj.rit_b1_;
      rit_b2_ = obj.rit_b2_;
      fit_b1_ = obj.fit_b1_;
      fit_b2_ = obj.fit_b2_;
      high_gain_ = obj.high_gain_;
      low_gain_ = obj.low_gain_;
      dyn_single_ = obj.dyn_single_;
   }

   void Clear(){
		event_number_ = 0;
		file_name_ = "";
      external_ts_high_ = 0;
      external_ts_low_ = 0;
      rit_b1_ = DEFAULT_BEAMLINE_DETECTOR_STRUC;
      rit_b2_ = DEFAULT_BEAMLINE_DETECTOR_STRUC;
      fit_b1_ = DEFAULT_BEAMLINE_DETECTOR_STRUC;
      fit_b2_ = DEFAULT_BEAMLINE_DETECTOR_STRUC;
      dyn_single_ = DEFAULT_BEAMLINE_DETECTOR_STRUC;

      high_gain_ = DEFAULT_PSPMT_STRUC;
      low_gain_ = DEFAULT_PSPMT_STRUC;
   }

	ULong64_t event_number_;
	std::string file_name_;
   ULong64_t external_ts_high_;
   ULong64_t external_ts_low_;
   beamline_detector_struc rit_b1_;
   beamline_detector_struc rit_b2_;
   beamline_detector_struc fit_b1_;
   beamline_detector_struc fit_b2_;
   beamline_detector_struc dyn_single_;
   pspmt_struc high_gain_;
   pspmt_struc low_gain_;
  
   ClassDef(PspmtData,6)
};


#endif
