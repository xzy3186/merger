#include "TraceAnalyzer.hpp"

int TraceAnalyzer::Configure(const std::string &yaml_node_name){
   /** loads configuration from yaml **/
   YamlReader yaml_reader(yaml_node_name);
   YAML::Node channel_list = yaml_reader.GetNode("ChannelList");

   for(auto channel : channel_list){
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
   for(auto channel : channel_vec_){
      channel.data_ = new TraceAnalyzerData();
      channel.tree_->Branch("trace_data","TraceAnalyzerData",channel.data_);
      std::cout << channel.data_ << std::endl;
   }
   return 0;
}

int TraceAnalyzer::Begin(){
   return 0;
}

int TraceAnalyzer::Process(const processor_struct::PSPMT &pspmt){
   for(auto channel: channel_vec_){
      if(pspmt.subtype.CompareTo(channel.subtype_.c_str())
         && pspmt.tag.CompareTo(channel.tag_.c_str())){
            channel.data_->Clear();
            std::cout << channel.data_ << std::endl;
            //channel.data_.pspmt_ = pspmt;
            //if(pspmt.trace.size()>100){
            //   /* subtract baseline */
            //   const Int_t kNBins = 20;
            //   Double_t baseline = 0;
            //   for(int i=0; i<kNBins; ++i){
            //      baseline += pspmt.trace.at(i);
            //   }
            //   baseline = baseline/(double)kNBins;
            //   /* QDC */
            //   Double_t qdc = 0;
            //   for(int i=55; i<70; ++i){
            //      qdc += pspmt.trace.at(i);
            //   }
            //   channel.data_.trace_energy_ = qdc;
            //}
            channel.tree_->Fill();
         }
   }
   return 0;
}

int TraceAnalyzer::Terminate(){
   for(auto channel: channel_vec_){
      channel.tree_->Write();
   }
}