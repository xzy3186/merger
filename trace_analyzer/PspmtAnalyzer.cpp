#include "PspmtAnalyzer.hpp"

int PspmtAnalyzer::Configure(const std::string &yaml_node_name){

   ResetConfig();

   /** loads configuration from the yaml file **/
   YamlReader yaml_reader(yaml_node_name);
   const std::string tree_name = yaml_reader.GetString("TreeName");
   output_tree_ = new TTree(tree_name.c_str(),tree_name.c_str());
   output_tree_->Branch("pspmt_compressed","PspmtData",&pspmt_data_);
   output_tree_->Branch("event_info_","event_info_struc",&event_info_);
   if(yaml_reader.GetBoolean("BATO",false,false))
      output_tree_->Branch("bato_vec_","std::vector<processor_struct::BATO>",&bato_data_);
   if(yaml_reader.GetBoolean("CLOVER",false,false))
      output_tree_->Branch("clover_vec_","std::vector<processor_struct::CLOVER>",&clover_data_);
   if(yaml_reader.GetBoolean("DOUBLEBETA",false,false))
      output_tree_->Branch("double_beta_vec_","std::vector<processor_struct::DOUBLEBETA>",&doublebeta_data_);
   if(yaml_reader.GetBoolean("GAMMASCINT",false,false))
      output_tree_->Branch("gamma_scint_vec_","std::vector<processor_struct::GAMMASCINT>",&gammascint_data_);
   if(yaml_reader.GetBoolean("LOGIC",false,false))
      output_tree_->Branch("logic_vec_","std::vector<processor_struct::LOGIC>",&logic_data_);
   if(yaml_reader.GetBoolean("NEXT",false,false))
      output_tree_->Branch("next_vec_","std::vector<processor_struct::NEXT>",&next_data_);
   if(yaml_reader.GetBoolean("PID",false,false))
      output_tree_->Branch("pid_vec_","std::vector<processor_struct::PID>",&pid_data_);
   if(yaml_reader.GetBoolean("ROOTDEV",false,false))
      output_tree_->Branch("root_dev_vec_","std::vector<processor_struct::ROOTDEV>",&rootdev_data_);
   if(yaml_reader.GetBoolean("SINGLEBETA",false,false))
      output_tree_->Branch("singlebeta_vec_","std::vector<processor_struct::SINGLEBETA>",&singlebeta_data_);
   if(yaml_reader.GetBoolean("VANDLE",false,false))
      output_tree_->Branch("vandle_vec_","std::vector<processor_struct::VANDLE>",&vandle_data_);

   kTWINDOW = yaml_reader.GetDouble("TimeWindow"); 
   kTOFFSET = yaml_reader.GetDouble("TimeOffset"); 
   kTWINDOW_ION = yaml_reader.GetDouble("TimeWindowIon");
   kTOFFSET_ION = yaml_reader.GetDouble("TimeOffsetIon");
   kTWINDOW_VETO = yaml_reader.GetDouble("TimeWindowVeto");
   kTOFFSET_VETO = yaml_reader.GetDouble("TimeOffsetVeto");

   kHIGH_GAIN_THRESHOLD = yaml_reader.GetDouble("HighGainThreshold",false,0);
   kHIGH_GAIN_OVERFLOW = yaml_reader.GetDouble("HighGainOverflow",false,4095);
   kLOW_GAIN_THRESHOLD = yaml_reader.GetDouble("LowGainThreshold",false,0);
   kLOW_GAIN_OVERFLOW = yaml_reader.GetDouble("LowGainOverflow",false,65535);

   auto get_correction = [&yaml_reader](std::string name) -> TF1* {
      auto formula = yaml_reader.GetString(name + "Formula",false,"no_correction");
      if (formula == "no_correction")
         return nullptr;
      auto tmp = new TF1(name.c_str(), formula.c_str(), -65535, 65535);
      tmp->SetParameters(yaml_reader.GetDoubleVec(name + "Parameters").data());
      return tmp;
   };

   fCorrectionHighGainXA = get_correction("HighGainXA");
   fCorrectionHighGainXB = get_correction("HighGainXB");
   fCorrectionHighGainYA = get_correction("HighGainYA");
   fCorrectionHighGainYB = get_correction("HighGainYB");
   fCorrectionLowGainXA = get_correction("LowGainXA");
   fCorrectionLowGainXB = get_correction("LowGainXB");
   fCorrectionLowGainYA = get_correction("LowGainYA");
   fCorrectionLowGainYB = get_correction("LowGainYB");
   fCorrectionHighGainPosX = get_correction("HighGainPosX");
   fCorrectionHighGainPosY = get_correction("HighGainPosY");
   fCorrectionLowGainPosX = get_correction("LowGainPosX");
   fCorrectionLowGainPosY = get_correction("LowGainPosY");

   fCorrectionHighGainPosX->Print();
   return 0;
}

int PspmtAnalyzer::ResetConfig() {

   if (output_tree_) {
      delete output_tree_;
      output_tree_ = nullptr;
   }
   auto delete_tf1 = [](TF1* f) {
      if (f) {
         delete f;
         f = nullptr;
         return f;
      }
   };
   fCorrectionHighGainXA = delete_tf1(fCorrectionHighGainXA);
   fCorrectionHighGainXB = delete_tf1(fCorrectionHighGainXB);
   fCorrectionHighGainYA = delete_tf1(fCorrectionHighGainYA);
   fCorrectionHighGainYB = delete_tf1(fCorrectionHighGainYB);
   fCorrectionLowGainXA = delete_tf1(fCorrectionLowGainXA);
   fCorrectionLowGainXB = delete_tf1(fCorrectionLowGainXB);
   fCorrectionLowGainYA = delete_tf1(fCorrectionLowGainYA);
   fCorrectionLowGainYB = delete_tf1(fCorrectionLowGainYB);
   fCorrectionHighGainPosX = delete_tf1(fCorrectionHighGainPosX);
   fCorrectionHighGainPosY = delete_tf1(fCorrectionHighGainPosY);
   fCorrectionLowGainPosX = delete_tf1(fCorrectionLowGainPosX);
   fCorrectionLowGainPosY = delete_tf1(fCorrectionLowGainPosY);
}

int PspmtAnalyzer::Begin(){
   return 0;
}

void PspmtAnalyzer::SetEventData(PixTreeEvent* pixie_event){
   
   event_info_.file_name_ = pixie_event->fileName;
   event_info_.pixie_event_num_ = pixie_event->eventNum;

   bato_data_ = pixie_event->bato_vec_;
   clover_data_ = pixie_event->clover_vec_;
   doublebeta_data_ = pixie_event->doublebeta_vec_;
   gammascint_data_ = pixie_event->gammascint_vec_;
   logic_data_ = pixie_event->logic_vec_;
   next_data_ = pixie_event->next_vec_;
   pid_data_ = pixie_event->pid_vec_;
   rootdev_data_ = pixie_event->rootdev_vec_;
   singlebeta_data_ = pixie_event->singlebeta_vec_;
   vandle_data_ = pixie_event->vandle_vec_;

   return;
}

int PspmtAnalyzer::Process(std::vector<processor_struct::PSPMT> &pspmt_vec,const ULong64_t ts){

	/* bad trace rejection */
	auto itr = pspmt_vec.begin();

   /* Get a vector of dynode_high singles events */
   std::vector<processor_struct::PSPMT> dynode_high_vec;
	std::string subtype = "";
	std::string tag = "";
   for(const auto &channel: pspmt_vec){
		subtype = channel.subtype.Data();
		tag = channel.tag.Data();
      if(!channel.subtype.CompareTo("dynode_high")&&!channel.tag.CompareTo("singles")){
		   dynode_high_vec.emplace_back(channel);
      }
   }

   /* loop through dynode high events and fill events to a tree */
   for(const auto &dyn_high: dynode_high_vec){
      data_.Clear();
      pspmt_data_.Clear();
      const Double_t t0 = dyn_high.time; // reference time
      pspmt_data_.dyn_single_.time_ = t0;
      pspmt_data_.dyn_single_.energy_ = dyn_high.energy;
		pspmt_data_.event_number_ = event_info_.pixie_event_num_;
		pspmt_data_.file_name_ = event_info_.file_name_;

      /* fills data to PspmtAnalyzerData */
      Int_t n_low_gain = 0;
      Int_t n_veto = 0;
      Int_t n_ion = 0;

		/* gate functions */
		const auto pspmt_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET) < kTWINDOW) )
				return true;
			else
				return false;
		};

		const auto veto_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_VETO) < kTWINDOW_VETO))
				return true;
			else
				return false;
		};

		const auto ion_gate = [&](const processor_struct::PSPMT &x) {
			if ((abs(x.time - t0 - kTOFFSET_ION) < kTWINDOW_ION))
				return true;
			else
				return false;
		};

		/* filling output data_ */
      for(const auto &channel: pspmt_vec){
         if(!channel.subtype.CompareTo("dynode_high")&&!channel.tag.CompareTo("ignore")){
            /* dynode high gain (triple) */
				if (pspmt_gate(channel))
					data_.high_gain_.dynode_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("dynode_low")&&!channel.tag.CompareTo("ignore")){
            /* dynode low gain */
				if(pspmt_gate(channel))
					data_.low_gain_.dynode_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("xa")){
            /* anode low gain xa */
				if(pspmt_gate(channel))
					data_.low_gain_.xa_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("xb")){
            /* anode low gain xb */
				if(pspmt_gate(channel))
					data_.low_gain_.xb_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("ya")){
            /* anode low gain ya */
				if(pspmt_gate(channel))
	            data_.low_gain_.ya_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_low")&&!channel.tag.CompareTo("yb")){
            /* anode low gain yb */
				if(pspmt_gate(channel))
					data_.low_gain_.yb_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("xa")){
            /* anode high gain xa */
				if(pspmt_gate(channel))
	            data_.high_gain_.xa_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("xb")){
            /* anode high gain xb */
				if(pspmt_gate(channel))
					data_.high_gain_.xb_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("ya")){
            /* anode high gain ya */
				if(pspmt_gate(channel))
					data_.high_gain_.ya_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("anode_high")&&!channel.tag.CompareTo("yb")){
            /* anode high gain yb */
				if(pspmt_gate(channel))
					data_.high_gain_.yb_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("white")){
            /* front plastic white */
				if(ion_gate(channel))
					data_.ion_white_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("green")){
            /* front plastic green */
				if(ion_gate(channel))
					data_.ion_green_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("blue")){
            /* front plastic blue */
				if(ion_gate(channel))
					data_.ion_blue_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("ion")&&!channel.tag.CompareTo("black")){
            /* front plastic black */
				if(ion_gate(channel))
					data_.ion_black_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("veto")&&!channel.tag.CompareTo("1")){
            /* veto plastic first */
				if(veto_gate(channel))
					data_.veto_first_.pspmt_ = channel;
         }
         else if(!channel.subtype.CompareTo("veto")&&!channel.tag.CompareTo("2")){
            /* veto plastic second */
				if(veto_gate(channel))
					data_.veto_second_.pspmt_ = channel;
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
		
      output_tree_->Fill();

   } // end of dynode high events loop

   return 0;
}

int PspmtAnalyzer::Terminate(){
   output_tree_->Write();
   return 0;
}

void PspmtAnalyzer::Correct(double& val, TF1* func) {
   if (!func)
      return;
   else
      val = func->Eval(val);
   return;
};

void PspmtAnalyzer::CalculatePositionH(pspmt_data_struc &data)
{
   /* pspmt position calculation */
   Correct(data.xa_.pspmt_.traceMaxVal,fCorrectionHighGainXA);
   Correct(data.xb_.pspmt_.traceMaxVal,fCorrectionHighGainXB);
   Correct(data.ya_.pspmt_.traceMaxVal,fCorrectionHighGainYA);
   Correct(data.yb_.pspmt_.traceMaxVal,fCorrectionHighGainYB);

   const double xa = data.xa_.pspmt_.traceMaxVal;
   const double xb = data.xb_.pspmt_.traceMaxVal;
   const double ya = data.ya_.pspmt_.traceMaxVal;
   const double yb = data.yb_.pspmt_.traceMaxVal;

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
   Correct(data.pos_x_,fCorrectionHighGainPosX);
   Correct(data.pos_y_,fCorrectionHighGainPosY);

   return;
}

void PspmtAnalyzer::CalculatePositionL(pspmt_data_struc &data)
{
	/* Correction of trace energy vs energy as a linear function of the ratio */
	/* (trace energy)/energy which express the effect of clipping by the fast amp */
	//const auto CorrectedTraceEnergy = [&](const processor_struct::PSPMT& x,const double a)
	//{
	//	//return x.traceMaxVal + kTRACEMAX_QDC_RATIO*x.traceMaxVal/x.energy;
	//	return x.traceMaxVal + a*(x.traceMaxVal/x.energy);
	//};

   //const double xa = CorrectedTraceEnergy(data.xa_.pspmt_,kTRACEMAX_QDC_RATIO_XA) + kLOW_GAIN_OFFSET_XA;
   //const double xb = CorrectedTraceEnergy(data.xb_.pspmt_,kTRACEMAX_QDC_RATIO_XB) + kLOW_GAIN_OFFSET_XB;
   //const double ya = CorrectedTraceEnergy(data.ya_.pspmt_,kTRACEMAX_QDC_RATIO_YA) + kLOW_GAIN_OFFSET_YA;
   //const double yb = CorrectedTraceEnergy(data.yb_.pspmt_,kTRACEMAX_QDC_RATIO_YB) + kLOW_GAIN_OFFSET_YB;

   /* pspmt position calculation */
   Correct(data.xa_.pspmt_.traceMaxVal,fCorrectionLowGainXA);
   Correct(data.xb_.pspmt_.traceMaxVal,fCorrectionLowGainXB);
   Correct(data.ya_.pspmt_.traceMaxVal,fCorrectionLowGainYA);
   Correct(data.yb_.pspmt_.traceMaxVal,fCorrectionLowGainYB);

   const double xa = data.xa_.pspmt_.traceMaxVal;
   const double xb = data.xb_.pspmt_.traceMaxVal;
   const double ya = data.ya_.pspmt_.traceMaxVal;
   const double yb = data.yb_.pspmt_.traceMaxVal;

	//pspmt_data_.low_gain_.xa_trace_energy_ = xa;
	//pspmt_data_.low_gain_.xb_trace_energy_ = xb;
	//pspmt_data_.low_gain_.ya_trace_energy_ = ya;
	//pspmt_data_.low_gain_.yb_trace_energy_ = yb;

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
   Correct(data.pos_x_,fCorrectionLowGainPosX);
   Correct(data.pos_y_,fCorrectionLowGainPosY);

   return;
}

