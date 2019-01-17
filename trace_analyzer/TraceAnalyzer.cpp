#include "TraceAnalyzer.hpp"
#include <algorithm>
int TraceAnalyzer::Configure(const std::string &yaml_node_name){
   /** loads configuration from yaml **/
   YamlReader yaml_reader(yaml_node_name);
   YAML::Node channel_list = yaml_reader.GetNode("ChannelList");

   for(const auto &channel : channel_list){
      std::cout << kMsgPrefix << channel << std::endl;
      YAML::Node node = channel["Channel"];
      const std::string tree_name = node["TreeName"].as<std::string>();
      const std::string subtype_name = node["SubtypeName"].as<std::string>();
      const std::string tag_name = node["TagName"].as<std::string>();
      parameter_struc struc;
      struc.tree_ = new TTree(tree_name.c_str(),tree_name.c_str());
      struc.subtype_ = subtype_name;
      struc.tag_ = tag_name;
      channel_vec_.emplace_back(struc);
   }
   for(auto &channel : channel_vec_){
      channel.data_ = new TraceAnalyzerData();
      channel.tree_->Branch("trace_data","TraceAnalyzerData",channel.data_);
   }

   return 0;
   }

   int TraceAnalyzer::Begin(){
   return 0;
   }

   int TraceAnalyzer::Process(const processor_struct::PSPMT &pspmt, const ULong64_t &ext){
   for(auto &channel: channel_vec_){
      
      if(!pspmt.subtype.CompareTo(channel.subtype_.c_str())
         && !pspmt.tag.CompareTo(channel.tag_.c_str())){
            channel.data_->Clear();
            channel.data_->pspmt_ = pspmt;
	    channel.data_->external_ts_ = ext;
	    /** implementation of trace analysis **/
            if(pspmt.trace.size()>100){
               /* subtract baseline */
               const Int_t kNBins = 20;
               Double_t baseline = 0;
               for(int i=0; i<kNBins; ++i){
                  baseline += pspmt.trace.at(i);
               }
               baseline = baseline/(double)kNBins;
               /* QDC */
               Double_t qdc = -999;
               for(int i=55; i<70; ++i){
                  qdc += pspmt.trace.at(i) - baseline;
               }
              // channel.data_->trace_energy_ = qdc;
               channel.data_->trace_max_ = (*std::max_element(pspmt.trace.begin(), pspmt.trace.end())); // getting the trace maximum 
               channel.data_->trace_energy_ = (*std::max_element(pspmt.trace.begin(), pspmt.trace.end())); // getting the trace maximum 
	     /** implementation of trace analysis **/
              }
            
            channel.data_->trace_size_ = pspmt.trace.size();
           if (pspmt.trace.size()!=0){
	       std::cout<<"size is: "<<channel.data_->trace_size_<<std::endl;
               // channel.data_->trace_max_ = (*std::max_element(pspmt.trace.begin(), pspmt.trace.end())); // getting the trace maximum 
	       // std::cout<<"come here"<<std::endl;
	       // std::cout<<"max is: "<<channel.data_->trace_max_<<std::endl;
                channel.data_vec_.emplace_back(*(channel.data_));
                channel.tree_->Fill();
            }
         }
         }
         return 0;
         }

         int TraceAnalyzer::Terminate(){
         for(auto &channel: channel_vec_){
         channel.tree_->Write();
         }
         }

         void TraceAnalyzer::ClearVec(){
         for(auto &channel: channel_vec_)
         channel.data_vec_.clear();
         }
