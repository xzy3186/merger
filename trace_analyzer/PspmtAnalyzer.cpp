#include "PspmtAnalyzer.hpp"

int PspmtAnalyzer::Configure(const std::string &yaml_node_name){
   /** loads configuration from the yaml file **/
   YamlReader yaml_reader(yaml_node_name);
   const std::string tree_name = yaml_reader.GetString("TreeName");
   output_tree_ = new TTree(tree_name.c_str(),tree_name.c_str());
   output_tree_->Branch("pspmt_compressed","PspmtData",&pspmt_data_);
   output_tree_->Branch("pixie_event_num",&pixie_event_num_,"pixie_event_num/l");
   output_tree_->Branch("file_name","TString", &file_name_);
   if(yaml_reader.GetBoolean("CloverVec",false,false))
      output_tree_->Branch("clover_vec_","processor_struct::CLOVERS",&clover_data_);
   if(yaml_reader.GetBoolean("VANDLEVec",false,false))
      output_tree_->Branch("vandle_vec_","processor_struct::VANDLES",&vandle_data_);
   if(yaml_reader.GetBoolean("GammaScintVec",false,false))
      output_tree_->Branch("gamma_scint_vec_","processor_struct::GAMMASCINT",&gamma_scint_data_);
   if(yaml_reader.GetBoolean("DoubleBetaVec",false,false))
      output_tree_->Branch("double_beta_vec_","processor_struct::DOUBLEBETA",&double_beta_data_);
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

   return 0;
}

int PspmtAnalyzer::Begin(){
// pos.open("pos.txt",std::fstream::out);
   return 0;
}

void PspmtAnalyzer::SetEventId(const TString &file_name, const ULong64_t event_num){
   file_name_ = file_name;
   pixie_event_num_ = event_num;
   return;
}

void PspmtAnalyzer::SetEventData(PixTreeEvent* pixie_event){
   clover_data_ = pixie_event->clover_vec_;
   vandle_data_ = pixie_event->vandle_vec_;
   gamma_scint_data_ = pixie_event->gamma_scint_vec_;
   double_beta_data_ = pixie_event->doublebeta_vec_;
   return;
}

int PspmtAnalyzer::Process(const std::vector<parameter_struc> &channel_data_vec){

   /* Get a vector of dynode_high singles events */
   std::vector<TraceAnalyzerData> dynode_high_vec;
   for(const auto &channel: channel_data_vec){
      if((channel.subtype_=="dynode_high")&&(channel.tag_=="singles")){
         dynode_high_vec = channel.data_vec_;
      }
   }

   /* loop through dynode high events and fill events to a tree */
   for(const auto &dyn_high: dynode_high_vec){
      data_.Clear();
      pspmt_data_.Clear();
      const Double_t t0 = dyn_high.pspmt_.time; // reference time
      pspmt_data_.dyn_single_.time_ = t0;
      pspmt_data_.dyn_single_.energy_ = dyn_high.pspmt_.energy;

      /* fills data to PspmtAnalyzerData */
      Int_t n_low_gain = 0;
      Int_t n_desi = 0;
      Int_t n_veto = 0;
      Int_t n_ion = 0;
      Int_t n_f11 = 0;
      for(const auto &channel: channel_data_vec){
         if((channel.subtype_=="dynode_high")&&(channel.tag_=="ignore")){
            /* dynode high gain (triple) */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.high_gain_.dynode_ = ch_data;
               }
            }
         }
         if((channel.subtype_=="dynode_low")&&(channel.tag_=="ignore")){
            /* dynode low gain */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.low_gain_.dynode_ = ch_data;
                  ++n_low_gain;
               }
            }
         }
         if((channel.subtype_=="anode_low")&&(channel.tag_=="xa")){
            /* anode low gain xa */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.low_gain_.xa_ = ch_data;
                  ++n_low_gain;
               }
            }
         }
         if((channel.subtype_=="anode_low")&&(channel.tag_=="xb")){
            /* anode low gain xb */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.low_gain_.xb_ = ch_data;
                  ++n_low_gain;
               }
            }
         }
         if((channel.subtype_=="anode_low")&&(channel.tag_=="ya")){
            /* anode low gain ya */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.low_gain_.ya_ = ch_data;
                  ++n_low_gain;
               }
            }
         }
         if((channel.subtype_=="anode_low")&&(channel.tag_=="yb")){
            /* anode low gain yb */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW){
                  data_.low_gain_.yb_ = ch_data;
                  ++n_low_gain;
               }
            }
         }
         if((channel.subtype_=="anode_high")&&(channel.tag_=="xa")){
            /* anode high gain xa */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW)
                  data_.high_gain_.xa_ = ch_data;
            }
         }
         if((channel.subtype_=="anode_high")&&(channel.tag_=="xb")){
            /* anode high gain xb */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW)
                  data_.high_gain_.xb_ = ch_data;
            }
         }
         if((channel.subtype_=="anode_high")&&(channel.tag_=="ya")){
            /* anode high gain ya */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW)
                  data_.high_gain_.ya_ = ch_data;
            }
         }
         if((channel.subtype_=="anode_high")&&(channel.tag_=="yb")){
            /* anode high gain yb */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET;
               if( tdiff > -kTWINDOW && tdiff < kTWINDOW)
                  data_.high_gain_.yb_ = ch_data;
            }
         }
         if((channel.subtype_=="desi")&&(channel.tag_=="top")){
            /* dE Si top */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_DESI;
               if( tdiff > -kTWINDOW_DESI && tdiff < kTWINDOW_DESI){
                  data_.desi_top_ = ch_data;
                  ++n_desi;
               }
            }
         }
         if((channel.subtype_=="desi")&&(channel.tag_=="bottom")){
            /* dE Si bottom */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_DESI;
               if( tdiff > -kTWINDOW_DESI && tdiff < kTWINDOW_DESI){
                  data_.desi_bottom_ = ch_data;
                  ++n_desi;
               }
            }
         }
         if((channel.subtype_=="ion")&&(channel.tag_=="white")){
            /* front plastic white */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_ION;
               if( tdiff > -kTWINDOW_ION && tdiff < kTWINDOW_ION){
                  data_.ion_white_ = ch_data;
                  ++n_ion;
               }
            }
         }
         if((channel.subtype_=="ion")&&(channel.tag_=="green")){
            /* front plastic green */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_ION;
               if( tdiff > -kTWINDOW_ION && tdiff < kTWINDOW_ION){
                  data_.ion_green_ = ch_data;
                  ++n_ion;
               }
            }
         }
         if((channel.subtype_=="ion")&&(channel.tag_=="blue")){
            /* front plastic blue */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_ION;
               if( tdiff > -kTWINDOW_ION && tdiff < kTWINDOW_ION){
                  data_.ion_blue_ = ch_data;
                  ++n_ion;
               }
            }
         }
         if((channel.subtype_=="ion")&&(channel.tag_=="black")){
            /* front plastic black */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_ION;
               if( tdiff > -kTWINDOW_ION && tdiff < kTWINDOW_ION){
                  data_.ion_black_ = ch_data;
                  ++n_ion;
               }
            }
         }
         if((channel.subtype_=="veto")&&(channel.tag_=="1")){
            /* veto plastic first */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_VETO;
               if( tdiff > -kTWINDOW_VETO && tdiff < kTWINDOW_VETO){
                  data_.veto_first_ = ch_data;
                  ++n_veto;
               }
            }
         }
         if((channel.subtype_=="veto")&&(channel.tag_=="2")){
            /* veto plastic second */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_VETO;
               if( tdiff > -kTWINDOW_VETO && tdiff < kTWINDOW_VETO){
                  data_.veto_second_ = ch_data;
                  ++n_veto;
               }
            }
         }
         if((channel.subtype_=="f11")&&(channel.tag_=="left")){
            /* f11 plastic left */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_F11;
               if( tdiff > -kTWINDOW_F11 && tdiff < kTWINDOW_F11){
                  data_.f11_left_ = ch_data;
                  ++n_f11;
               }
            }
         }
         if((channel.subtype_=="f11")&&(channel.tag_=="right")){
            /* f11 plastic right */
            for(const auto &ch_data: channel.data_vec_){
               const Double_t tdiff = ch_data.pspmt_.time - t0 - kTOFFSET_F11;
               if( tdiff > -kTWINDOW_F11 && tdiff < kTWINDOW_F11){
                  data_.f11_right_ = ch_data;
                  ++n_f11;
               }
            }
         }
      }

      /* position analysis */
      if(1){
         /* high gain */
         data_.external_ts_high_ = data_.high_gain_.dynode_.external_ts_;
         pspmt_data_.external_ts_high_ = data_.high_gain_.dynode_.external_ts_;
         pspmt_data_.high_gain_.trace_energy_ = data_.high_gain_.dynode_.trace_energy_;
         pspmt_data_.high_gain_.energy_ = data_.high_gain_.dynode_.pspmt_.energy;
         pspmt_data_.high_gain_.time_ = data_.high_gain_.dynode_.pspmt_.time;
         pspmt_data_.high_gain_.energy_sum_ = data_.high_gain_.xa_.trace_energy_
            + data_.high_gain_.xb_.trace_energy_ + data_.high_gain_.ya_.trace_energy_
            + data_.high_gain_.yb_.trace_energy_;

         CalculatePositionH(data_.high_gain_);
         pspmt_data_.high_gain_.pos_x_= data_.high_gain_.pos_x_;
         pspmt_data_.high_gain_.pos_y_= data_.high_gain_.pos_y_;
         pspmt_data_.high_gain_.valid_= data_.high_gain_.valid_;

      }
      if(n_low_gain){
         /* low gain */
         data_.external_ts_low_ = data_.low_gain_.dynode_.external_ts_;
         pspmt_data_.external_ts_low_ = data_.low_gain_.dynode_.external_ts_;
         pspmt_data_.low_gain_.trace_energy_ = data_.low_gain_.dynode_.trace_energy_;
         pspmt_data_.low_gain_.energy_ = data_.low_gain_.dynode_.pspmt_.energy;
         pspmt_data_.low_gain_.time_ = data_.low_gain_.dynode_.pspmt_.time;
         pspmt_data_.low_gain_.energy_sum_ = data_.low_gain_.xa_.trace_energy_
            + data_.low_gain_.xb_.trace_energy_ + data_.low_gain_.ya_.trace_energy_
            + data_.low_gain_.yb_.trace_energy_;

         CalculatePositionL(data_.low_gain_);
         pspmt_data_.low_gain_.pos_x_= data_.low_gain_.pos_x_;
         pspmt_data_.low_gain_.pos_y_= data_.low_gain_.pos_y_;
         pspmt_data_.low_gain_.valid_= data_.low_gain_.valid_;
      }
      if(n_desi){
         pspmt_data_.desi_top_.energy_ = data_.desi_top_.pspmt_.energy;
         pspmt_data_.desi_bottom_.energy_ = data_.desi_bottom_.pspmt_.energy;
         pspmt_data_.desi_top_.time_ = data_.desi_top_.pspmt_.time;
         pspmt_data_.desi_bottom_.time_ = data_.desi_bottom_.pspmt_.time;
      }
      if(n_veto){
         pspmt_data_.veto_first_.energy_ = data_.veto_first_.pspmt_.energy;
         pspmt_data_.veto_second_.energy_ = data_.veto_second_.pspmt_.energy;
         pspmt_data_.veto_first_.time_ = data_.veto_first_.pspmt_.time;
         pspmt_data_.veto_second_.time_ = data_.veto_second_.pspmt_.time;
      }
      if(n_ion){
         pspmt_data_.ion_white_.energy_ = data_.ion_white_.pspmt_.energy;
         pspmt_data_.ion_green_.energy_ = data_.ion_green_.pspmt_.energy;
         pspmt_data_.ion_blue_.energy_ = data_.ion_blue_.pspmt_.energy;
         pspmt_data_.ion_black_.energy_ = data_.ion_black_.pspmt_.energy;
         pspmt_data_.ion_white_.time_ = data_.ion_white_.pspmt_.time;
         pspmt_data_.ion_green_.time_ = data_.ion_green_.pspmt_.time;
         pspmt_data_.ion_blue_.time_ = data_.ion_blue_.pspmt_.time;
         pspmt_data_.ion_black_.time_ = data_.ion_black_.pspmt_.time;
      }
      if(n_f11){
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
   pos.close();
   output_tree_->Write();
   return 0;
}

void PspmtAnalyzer::CalculatePositionH(pspmt_data_struc &data)
{
   /* pspmt position calculation */
   const double xa = data.xa_.trace_energy_;
   const double xb = data.xb_.trace_energy_;
   const double ya = data.ya_.trace_energy_;
   const double yb = data.yb_.trace_energy_;
   const double sum = xa + xb + ya + yb;
   if(sum<0){
      data.valid_=0;
      return;
   }

   data.pos_x_ = 0.5*(yb + xa)/sum;
   data.pos_y_ = 0.5*(xa + xb)/sum;
   if(xa>0&&xb>0&&ya>0&&yb>0){
      data.valid_ = 1;
   }else
      data.valid_ = 0;
   return;
}

void PspmtAnalyzer::CalculatePositionL(pspmt_data_struc &data)
{
   const double xa =4096.0*(exp((data.xa_.trace_energy_)/(3000))-1);
   const double xb =4096.0*(exp((data.xb_.trace_energy_)/(3000))-1);
   const double ya =4096.0*(exp((data.ya_.trace_energy_)/(3000))-1);
   const double yb =4096.0*(exp((data.yb_.trace_energy_)/(3000))-1);
   const double dynode_trace = data.dynode_.trace_energy_;
   const double sum = xa + xb + ya + yb;
      if(sum<0){
         data.valid_=0;
         return;
      }  

   data.pos_x_ = 0.5*(yb + xa)/sum;
   data.pos_y_ = 0.5*(xa + xb)/sum;
   if(xa>0&&xb>0&&ya>0&&yb>0){
      data.valid_ = 1;
   }
   else
      data.valid_ = 0;
   return;
}

