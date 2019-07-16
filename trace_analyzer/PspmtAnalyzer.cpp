#include "PspmtAnalyzer.hpp"

int PspmtAnalyzer::Configure(const std::string &yaml_node_name){
   /** loads configuration from the yaml file **/
   YamlReader yaml_reader(yaml_node_name);
   const std::string tree_name = yaml_reader.GetString("TreeName");
   output_tree_ = new TTree(tree_name.c_str(),tree_name.c_str());
   output_tree_->Branch("pspmt_compressed","PspmtData",&pspmt_data_);
   output_tree_->Branch("event_info","event_info_struc",&event_info_);
   if(yaml_reader.GetBoolean("CloverVec",false,false))
      output_tree_->Branch("clover_vec_","std::vector<processor_struct::CLOVERS>",&clover_data_);
   if(yaml_reader.GetBoolean("VANDLEVec",false,false))
      output_tree_->Branch("vandle_vec_","std::vector<processor_struct::VANDLES>",&vandle_data_);
   if(yaml_reader.GetBoolean("GammaScintVec",false,false))
      output_tree_->Branch("gamma_scint_vec_","std::vector<processor_struct::GAMMASCINT>",&gamma_scint_data_);
   if(yaml_reader.GetBoolean("DoubleBetaVec",false,false))
      output_tree_->Branch("double_beta_vec_","std::vector<processor_struct::DOUBLEBETA>",&double_beta_data_);

   kTWINDOW = yaml_reader.GetDouble("TimeWindow"); 
   kTOFFSET = yaml_reader.GetDouble("TimeOffset"); 
   kTWINDOW_DESI = yaml_reader.GetDouble("TimeWindowDeSi");
   kTOFFSET_DESI = yaml_reader.GetDouble("TimeOffsetDeSi");
   kTWINDOW_ION = yaml_reader.GetDouble("TimeWindowIon");
   kTOFFSET_ION = yaml_reader.GetDouble("TimeOffsetIon");
   kTWINDOW_VETO = yaml_reader.GetDouble("TimeWindowVeto");
   kTOFFSET_VETO = yaml_reader.GetDouble("TimeOffsetVeto");
   kTWINDOW_F11 = yaml_reader.GetDouble("TimeWindowF11");
   kTOFFSET_F11 = yaml_reader.GetDouble("TimeOffsetF11");

   kHIGH_GAIN_THRESHOLD = yaml_reader.GetDouble("HighGainThreshold",false,0);
   kHIGH_GAIN_OVERFLOW = yaml_reader.GetDouble("HighGainOverflow",false,4095);
   kLOW_GAIN_THRESHOLD = yaml_reader.GetDouble("LowGainThreshold",false,0);
   kLOW_GAIN_OVERFLOW = yaml_reader.GetDouble("LowGainOverflow",false,65535);

	kHIGH_GAIN_OFFSET_XA = yaml_reader.GetDouble("HighGainOffsetXA", false, 0);
	kHIGH_GAIN_OFFSET_XB = yaml_reader.GetDouble("HighGainOffsetXB", false, 0);
	kHIGH_GAIN_OFFSET_YA = yaml_reader.GetDouble("HighGainOffsetYA", false, 0);
	kHIGH_GAIN_OFFSET_YB = yaml_reader.GetDouble("HighGainOffsetYB", false, 0);
	kLOW_GAIN_OFFSET_XA = yaml_reader.GetDouble("LowGainOffsetXA", false, 0);
	kLOW_GAIN_OFFSET_XB = yaml_reader.GetDouble("LowGainOffsetXB", false, 0);
	kLOW_GAIN_OFFSET_YA = yaml_reader.GetDouble("LowGainOffsetYA", false, 0);
	kLOW_GAIN_OFFSET_YB = yaml_reader.GetDouble("LowGainOffsetYB", false, 0);
	kTRACEMAX_QDC_RATIO_XA = yaml_reader.GetDouble("TraceMaxQDCRatioXA", false, 0);
	kTRACEMAX_QDC_RATIO_XB = yaml_reader.GetDouble("TraceMaxQDCRatioXB", false, 0);
	kTRACEMAX_QDC_RATIO_YA = yaml_reader.GetDouble("TraceMaxQDCRatioYA", false, 0);
	kTRACEMAX_QDC_RATIO_YB = yaml_reader.GetDouble("TraceMaxQDCRatioYB", false, 0);

   return 0;
}

int PspmtAnalyzer::Begin(){
   return 0;
}

void PspmtAnalyzer::SetEventId(const TString &file_name, const ULong64_t event_num){
   event_info_.file_name_ = file_name;
   event_info_.pixie_event_num_ = event_num;
   return;
}

void PspmtAnalyzer::SetEventData(PixTreeEvent* pixie_event){
   clover_data_ = pixie_event->clover_vec_;
   vandle_data_ = pixie_event->vandle_vec_;
   gamma_scint_data_ = pixie_event->gamma_scint_vec_;
   double_beta_data_ = pixie_event->doublebeta_vec_;
   return;
}

int PspmtAnalyzer::Process(std::vector<processor_struct::PSPMT> &pspmt_vec,const ULong64_t ts){

	/* bad trace rejection */
	auto itr = pspmt_vec.begin();
	//while (itr!=pspmt_vec.end()){
	//	itr = std::find_if(++itr, pspmt_vec.end(),
	//		[](const processor_struct::PSPMT & x) {
	//			return (x.traceMaxPos > 70 || x.traceMaxPos < 40); //|| x.invalidTrace);
	//		});
	//	if (itr != pspmt_vec.end())
	//		(*itr).invalidTrace = true;
	//}

   /* Get a vector of dynode_high singles events */
   std::vector<processor_struct::PSPMT> dynode_high_vec;
	std::string subtype = "";
	std::string tag = "";
   for(const auto &channel: pspmt_vec){
		//if (channel.invalidTrace)
		//	continue;
		subtype = channel.subtype.Data();
		tag = channel.tag.Data();
      if(!channel.subtype.CompareTo("dynode_high")&&!channel.tag.CompareTo("singles")){
		   dynode_high_vec.push_back(channel);
      }
   }

   /* loop through dynode high events and fill events to a tree */
   for(const auto &dyn_high: dynode_high_vec){
      data_.Clear();
      pspmt_data_.Clear();
      const Double_t t0 = dyn_high.time; // reference time
      pspmt_data_.dyn_single_.time_ = t0;
      pspmt_data_.dyn_single_.energy_ = dyn_high.energy;

      /* fills data to PspmtAnalyzerData */
      Int_t n_low_gain = 0;
      Int_t n_desi = 0;
      Int_t n_veto = 0;
      Int_t n_ion = 0;
      Int_t n_f11 = 0;

		/* gate functions */
		const auto &pspmt_gate = [&](const processor_struct::PSPMT &x) {
			//if ((abs(x.time - t0 - kTOFFSET) < kTWINDOW) )//&& !x.invalidTrace)
			//if ((abs(x.time - t0 - kTOFFSET) < kTWINDOW) && x.traceMaxPos<70&&x.traceMaxPos>40)
			if ((abs(x.time - t0 - kTOFFSET) < kTWINDOW) )
				return x;
			else
				return processor_struct::PSPMT_DEFAULT_STRUCT;
		};

		const auto &desi_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_DESI) < kTWINDOW_DESI))
				return x;
			else
				return processor_struct::PSPMT_DEFAULT_STRUCT;
		};

		const auto &veto_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_VETO) < kTWINDOW_VETO))
				return x;
			else
				return processor_struct::PSPMT_DEFAULT_STRUCT;
		};

		const auto &ion_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_ION) < kTWINDOW_ION))
				return x;
			else
				return processor_struct::PSPMT_DEFAULT_STRUCT;
		};

		const auto &f11_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_F11) < kTWINDOW_F11))
				return x;
			else
				return processor_struct::PSPMT_DEFAULT_STRUCT;
		};

		/* filling output data_ */
      for(const auto &channel: pspmt_vec){
         if(!channel.subtype.CompareTo("dynode_high")&&!channel.tag.CompareTo("ignore")){
            /* dynode high gain (triple) */
            data_.high_gain_.dynode_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("dynode_low")&&!channel.tag.CompareTo("ignore")){
            /* dynode low gain */
            data_.low_gain_.dynode_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("xa")){
            /* anode low gain xa */
            data_.low_gain_.xa_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("xb")){
            /* anode low gain xb */
            data_.low_gain_.xb_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("ya")){
            /* anode low gain ya */
            data_.low_gain_.ya_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("yb")){
            /* anode low gain yb */
            data_.low_gain_.yb_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("xa")){
            /* anode high gain xa */
            data_.high_gain_.xa_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("xb")){
            /* anode high gain xb */
            data_.high_gain_.xb_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("ya")){
            /* anode high gain ya */
            data_.high_gain_.ya_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("yb")){
            /* anode high gain yb */
            data_.high_gain_.yb_.pspmt_ = pspmt_gate(channel);
         }
         else if(!channel.subtype.CompareTo("desi")&&!channel.tag.CompareTo("top")){
            /* dE Si top */
            data_.desi_top_.pspmt_ = desi_gate(channel);
         }
         else if(!channel.subtype.CompareTo("desi")&&!channel.tag.CompareTo("bottom")){
            /* dE Si bottom */
            data_.desi_bottom_.pspmt_ = desi_gate(channel);
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("white")){
            /* front plastic white */
            data_.ion_white_.pspmt_ = ion_gate(channel);
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("green")){
            /* front plastic green */
            data_.ion_green_.pspmt_ = ion_gate(channel);
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("blue")){
            /* front plastic blue */
            data_.ion_blue_.pspmt_ = ion_gate(channel);
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("black")){
            /* front plastic black */
            data_.ion_black_.pspmt_ = ion_gate(channel);
         }
         else if(!channel.subtype.CompareTo("veto")&&!channel.tag.CompareTo("1")){
            /* veto plastic first */
            data_.veto_first_.pspmt_ = veto_gate(channel);
         }
         else if(!channel.subtype.CompareTo("veto")&&!channel.tag.CompareTo("2")){
            /* veto plastic second */
            data_.veto_second_.pspmt_ = veto_gate(channel);
         }
         else if(!channel.subtype.CompareTo("f11")&&!channel.tag.CompareTo("left")){
            /* f11 plastic left */
            data_.f11_left_.pspmt_ = f11_gate(channel);
         }
         else if(!channel.subtype.CompareTo("f11")&&!channel.tag.CompareTo("right")){
            /* f11 plastic right */
            data_.f11_right_.pspmt_ = f11_gate(channel);
         }
      }

      /* position analysis */
      {
         /* high gain */
         pspmt_data_.external_ts_high_ = ts;
         pspmt_data_.high_gain_.trace_energy_ = data_.high_gain_.dynode_.pspmt_.traceMaxVal;
         pspmt_data_.high_gain_.energy_ = data_.high_gain_.dynode_.pspmt_.energy;
         pspmt_data_.high_gain_.time_ = data_.high_gain_.dynode_.pspmt_.time;
         pspmt_data_.high_gain_.energy_sum_ = data_.high_gain_.xa_.pspmt_.traceMaxVal
            + data_.high_gain_.xb_.pspmt_.traceMaxVal + data_.high_gain_.ya_.pspmt_.traceMaxVal
            + data_.high_gain_.yb_.pspmt_.traceMaxVal;

         CalculatePositionH(data_.high_gain_);
         pspmt_data_.high_gain_.pos_x_= data_.high_gain_.pos_x_;
         pspmt_data_.high_gain_.pos_y_= data_.high_gain_.pos_y_;
         pspmt_data_.high_gain_.valid_= data_.high_gain_.valid_;

			pspmt_data_.high_gain_.xa_energy_ = data_.high_gain_.xa_.pspmt_.energy;
			pspmt_data_.high_gain_.xb_energy_ = data_.high_gain_.xb_.pspmt_.energy;
			pspmt_data_.high_gain_.ya_energy_ = data_.high_gain_.ya_.pspmt_.energy;
			pspmt_data_.high_gain_.yb_energy_ = data_.high_gain_.yb_.pspmt_.energy;
			pspmt_data_.high_gain_.xa_trace_energy_ = data_.high_gain_.xa_.pspmt_.traceMaxVal;
			pspmt_data_.high_gain_.xb_trace_energy_ = data_.high_gain_.xb_.pspmt_.traceMaxVal;
			pspmt_data_.high_gain_.ya_trace_energy_ = data_.high_gain_.ya_.pspmt_.traceMaxVal;
			pspmt_data_.high_gain_.yb_trace_energy_ = data_.high_gain_.yb_.pspmt_.traceMaxVal;

      }
      {
         /* low gain */
         data_.external_ts_low_ = ts;
         pspmt_data_.external_ts_low_ = ts;
         pspmt_data_.low_gain_.trace_energy_ = data_.low_gain_.dynode_.pspmt_.traceMaxVal;
         pspmt_data_.low_gain_.energy_ = data_.low_gain_.dynode_.pspmt_.energy;
         pspmt_data_.low_gain_.time_ = data_.low_gain_.dynode_.pspmt_.time;
         pspmt_data_.low_gain_.energy_sum_ = data_.low_gain_.xa_.pspmt_.traceMaxVal
            + data_.low_gain_.xb_.pspmt_.traceMaxVal + data_.low_gain_.ya_.pspmt_.traceMaxVal
            + data_.low_gain_.yb_.pspmt_.traceMaxVal;

         CalculatePositionL(data_.low_gain_);
         pspmt_data_.low_gain_.pos_x_= data_.low_gain_.pos_x_;
         pspmt_data_.low_gain_.pos_y_= data_.low_gain_.pos_y_;
         pspmt_data_.low_gain_.valid_= data_.low_gain_.valid_;

			pspmt_data_.low_gain_.xa_energy_ = data_.low_gain_.xa_.pspmt_.energy;
			pspmt_data_.low_gain_.xb_energy_ = data_.low_gain_.xb_.pspmt_.energy;
			pspmt_data_.low_gain_.ya_energy_ = data_.low_gain_.ya_.pspmt_.energy;
			pspmt_data_.low_gain_.yb_energy_ = data_.low_gain_.yb_.pspmt_.energy;
      }
      {
			/* dE Si */
         pspmt_data_.desi_top_.energy_ = data_.desi_top_.pspmt_.energy;
         pspmt_data_.desi_bottom_.energy_ = data_.desi_bottom_.pspmt_.energy;
         pspmt_data_.desi_top_.time_ = data_.desi_top_.pspmt_.time;
         pspmt_data_.desi_bottom_.time_ = data_.desi_bottom_.pspmt_.time;
      }
      {
			/* VETO */
         pspmt_data_.veto_first_.energy_ = data_.veto_first_.pspmt_.energy;
         pspmt_data_.veto_second_.energy_ = data_.veto_second_.pspmt_.energy;
         pspmt_data_.veto_first_.time_ = data_.veto_first_.pspmt_.time;
         pspmt_data_.veto_second_.time_ = data_.veto_second_.pspmt_.time;
      }
      {
			/* ION (front plastic) */
         pspmt_data_.ion_white_.energy_ = data_.ion_white_.pspmt_.energy;
         pspmt_data_.ion_green_.energy_ = data_.ion_green_.pspmt_.energy;
         pspmt_data_.ion_blue_.energy_ = data_.ion_blue_.pspmt_.energy;
         pspmt_data_.ion_black_.energy_ = data_.ion_black_.pspmt_.energy;
         pspmt_data_.ion_white_.time_ = data_.ion_white_.pspmt_.time;
         pspmt_data_.ion_green_.time_ = data_.ion_green_.pspmt_.time;
         pspmt_data_.ion_blue_.time_ = data_.ion_blue_.pspmt_.time;
         pspmt_data_.ion_black_.time_ = data_.ion_black_.pspmt_.time;
      }
      {
			/* F11 plastic */
         pspmt_data_.f11_left_.energy_ = data_.f11_left_.pspmt_.energy;
         pspmt_data_.f11_right_.energy_ = data_.f11_right_.pspmt_.energy;
         pspmt_data_.f11_left_.time_ = data_.f11_left_.pspmt_.time;
         pspmt_data_.f11_right_.time_ = data_.f11_right_.pspmt_.time;
      }
      output_tree_->Fill();

   } // end of dynode high events loop

   return 0;
}

int PspmtAnalyzer::Terminate(){
   output_tree_->Write();
   return 0;
}

void PspmtAnalyzer::CalculatePositionH(pspmt_data_struc &data)
{
   /* pspmt position calculation */
   const double xa = data.xa_.pspmt_.traceMaxVal + kHIGH_GAIN_OFFSET_XA;
   const double xb = data.xb_.pspmt_.traceMaxVal + kHIGH_GAIN_OFFSET_XB;
   const double ya = data.ya_.pspmt_.traceMaxVal + kHIGH_GAIN_OFFSET_YA;
   const double yb = data.yb_.pspmt_.traceMaxVal + kHIGH_GAIN_OFFSET_YB;

   /** check if all four anode signals are good **/
   if(
      xa>kHIGH_GAIN_THRESHOLD &&
      xb>kHIGH_GAIN_THRESHOLD &&
      ya>kHIGH_GAIN_THRESHOLD &&
      yb>kHIGH_GAIN_THRESHOLD &&
      xa<kHIGH_GAIN_OVERFLOW &&
      xb<kHIGH_GAIN_OVERFLOW &&
      ya<kHIGH_GAIN_OVERFLOW &&
      yb<kHIGH_GAIN_OVERFLOW
   ){
      data.valid_ = 1;
   }
   else{
      data.valid_ = 0;
      return;
   }

   /** position calculation **/
   const double sum = xa + xb + ya + yb;
   data.pos_x_ = 0.5*(yb + xa)/sum;
   data.pos_y_ = 0.5*(xa + xb)/sum;
   data.pos_x_ = 10.*data.pos_x_;
   data.pos_y_ = 10.*data.pos_y_;
   return;
}

void PspmtAnalyzer::CalculatePositionL(pspmt_data_struc &data)
{
	/* Correction of trace energy vs energy as a linear function of the ratio */
	/* (trace energy)/energy which express the effect of clipping by the fast amp */
	const auto CorrectedTraceEnergy = [&](const processor_struct::PSPMT& x,const double a)
	{
		//return x.traceMaxVal + kTRACEMAX_QDC_RATIO*x.traceMaxVal/x.energy;
		return x.traceMaxVal + a*(x.traceMaxVal/x.energy);
	};

   const double xa = CorrectedTraceEnergy(data.xa_.pspmt_,kTRACEMAX_QDC_RATIO_XA) + kLOW_GAIN_OFFSET_XA;
   const double xb = CorrectedTraceEnergy(data.xb_.pspmt_,kTRACEMAX_QDC_RATIO_XB) + kLOW_GAIN_OFFSET_XB;
   const double ya = CorrectedTraceEnergy(data.ya_.pspmt_,kTRACEMAX_QDC_RATIO_YA) + kLOW_GAIN_OFFSET_YA;
   const double yb = CorrectedTraceEnergy(data.yb_.pspmt_,kTRACEMAX_QDC_RATIO_YB) + kLOW_GAIN_OFFSET_YB;

	pspmt_data_.low_gain_.xa_trace_energy_ = xa;
	pspmt_data_.low_gain_.xb_trace_energy_ = xb;
	pspmt_data_.low_gain_.ya_trace_energy_ = ya;
	pspmt_data_.low_gain_.yb_trace_energy_ = yb;

   /** check if all four anode signals are good **/
   if(
      xa>kLOW_GAIN_THRESHOLD &&
      xb>kLOW_GAIN_THRESHOLD &&
      ya>kLOW_GAIN_THRESHOLD &&
      yb>kLOW_GAIN_THRESHOLD &&
      xa<kLOW_GAIN_OVERFLOW &&
      xb<kLOW_GAIN_OVERFLOW &&
      ya<kLOW_GAIN_OVERFLOW &&
      yb<kLOW_GAIN_OVERFLOW
   ){
      data.valid_ = 1;
   }
   else{
      data.valid_ = 0;
      return;
   }

   /** position calculation **/
   const double sum = xa + xb + ya + yb;
   data.pos_x_ = 0.5*(yb + xa)/sum;
   data.pos_y_ = 0.5*(xa + xb)/sum;
   data.pos_x_ = 10*2.76717*data.pos_x_-4.39413551;
   data.pos_y_ = 10*1.79261*data.pos_y_-1.90999;
   return;
}

