#include "PspmtAnalyzer.hpp"

int PspmtAnalyzer::Configure(const std::string &yaml_node_name){
   /** loads configuration from yaml **/
   YamlReader yaml_reader(yaml_node_name);
   const std::string tree_name = yaml_reader.GetString("TreeName");
   output_tree_ = new TTree(tree_name.c_str(),tree_name.c_str());
   output_tree_->Branch(tree_name.c_str(),"PspmtAnalyzerData",&data_);

   return 0;
}

int PspmtAnalyzer::Begin(){
   return 0;
}

int PspmtAnalyzer::Process(const std::vector<parameter_struc> &channel_data_vec){

   data_.Clear();
   /* fills data to PspmtAnalyzerData */
   Int_t n_high_gain = 0;
   Int_t n_low_gain = 0;
   Int_t n_desi = 0;
   for(const auto &channel: channel_data_vec){
      if((channel.subtype_=="dynode_high")&&(channel.tag_=="ignore")){
         /* dynode high gain */
         if(!channel.data_vec_.empty()){
            data_.high_gain_.dynode_ = channel.data_vec_.at(0);
            ++n_high_gain;
         }
      }
      if((channel.subtype_=="anode_high")&&(channel.tag_=="xa")){
         /* anode xa high gain */
         if(!channel.data_vec_.empty()){
            data_.high_gain_.xa_ = channel.data_vec_.at(0);
            ++n_high_gain;
         }
      }
      if((channel.subtype_=="anode_high")&&(channel.tag_=="xb")){
         /* anode xb high gain */
         if(!channel.data_vec_.empty()){
            data_.high_gain_.xb_ = channel.data_vec_.at(0);
            ++n_high_gain;
         }
      }
      if((channel.subtype_=="anode_high")&&(channel.tag_=="ya")){
         /* anode ya high gain */
         if(!channel.data_vec_.empty()){
            data_.high_gain_.ya_ = channel.data_vec_.at(0);
            ++n_high_gain;
         }
      }
      if((channel.subtype_=="anode_high")&&(channel.tag_=="yb")){
         /* anode yb high gain */
         if(!channel.data_vec_.empty()){
            data_.high_gain_.yb_ = channel.data_vec_.at(0);
            ++n_high_gain;
         }
      }
      if((channel.subtype_=="dynode_low")&&(channel.tag_=="ignore")){
         /* dynode low gain */
         if(!channel.data_vec_.empty()){
            data_.low_gain_.dynode_ = channel.data_vec_.at(0);
            ++n_low_gain;
         }
      }
      if((channel.subtype_=="anode_low")&&(channel.tag_=="xa")){
         /* anode xa low gain */
         if(!channel.data_vec_.empty()){
            data_.low_gain_.xa_ = channel.data_vec_.at(0);
            ++n_low_gain;
         }
      }
      if((channel.subtype_=="anode_low")&&(channel.tag_=="xb")){
         /* anode xb low gain */
         if(!channel.data_vec_.empty()){
            data_.low_gain_.xb_ = channel.data_vec_.at(0);
            ++n_low_gain;
         }
      }
      if((channel.subtype_=="anode_low")&&(channel.tag_=="ya")){
         /* anode ya low gain */
         if(!channel.data_vec_.empty()){
            data_.low_gain_.ya_ = channel.data_vec_.at(0);
            ++n_low_gain;
         }
      }
      if((channel.subtype_=="anode_low")&&(channel.tag_=="yb")){
         /* anode yb low gain */
         if(!channel.data_vec_.empty()){
            data_.low_gain_.yb_ = channel.data_vec_.at(0);
            ++n_low_gain;
         }
      }
      if((channel.subtype_=="desi")&&(channel.tag_=="top")){
         /* dE Si top */
         if(!channel.data_vec_.empty()){
            data_.desi_top_ = channel.data_vec_.at(0);
            ++n_desi;
         }
      }
      if((channel.subtype_=="desi")&&(channel.tag_=="bottom")){
         /* dE Si bottom */
         if(!channel.data_vec_.empty()){
            data_.desi_bottom_ = channel.data_vec_.at(0);
            ++n_desi;
         }
      }
   }

   /* position analysis */
   if(1){
      /* high gain */
      data_.external_ts_high_ = data_.high_gain_.dynode_.external_ts_;
      CalculatePosition(data_.high_gain_);
   }
   if(1){
      /* low gain */
      data_.external_ts_low_ = data_.low_gain_.dynode_.external_ts_;
      CalculatePosition(data_.low_gain_);
   }
   output_tree_->Fill();
   return 0;
}

int PspmtAnalyzer::Terminate(){
   output_tree_->Write();
   return 0;
}

void PspmtAnalyzer::CalculatePosition(pspmt_data_struc &data)
{
   /* pspmt position calculation */
   const double xa = data.xa_.trace_energy_;
   const double xb = data.xb_.trace_energy_;
   const double ya = data.ya_.trace_energy_;
   const double yb = data.yb_.trace_energy_;
   const double sum = xa + xb + ya + yb;
   if(sum<0)
      return;

   data.pos_x_ = 0.5*(yb + xa)/sum;
   data.pos_y_ = 0.5*(xa + xb)/sum;
   if(xa>0&&xb>0&&ya>0&&yb>0){
      data.valid_ = 1;
   }
   return;
}