#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TSelector.h"
#include "Rtypes.h"
#include "TMath.h"

#include "ProcessorRootStruc.hpp"
#include "DumpTreeData.h"


int main(int argc, char **argv){
   std::cout<<"input file: "<<argv[1]<<std::endl;
   TFile file(argv[1]);
   TTree *tree = (TTree*)file.Get("PixTree");
   ULong64_t total_entry = tree->GetEntries();
   std::cout<<"input tree entries: "<<total_entry<<std::endl;

   TTreeReader tree_reader(tree);
   TTreeReaderValue<PixTreeEvent> pixie_event_reader_(tree_reader, "PixTreeEvent");

   std::cout<<"output file: "<<argv[2]<<std::endl;
   TFile output_file_(argv[2],"RECREATE");

   DumpTreeData dump_data_beta_;    
   TTree *beta_tree_ = new TTree("beta_tree","beta_tree");
   beta_tree_->Branch("BetaTreeData","DumpTreeData",&dump_data_beta_);
   
   DumpTreeData dump_data_imp_;    
   TTree *implant_tree_ = new TTree("implant_tree","implant_tree");
   implant_tree_->Branch("ImplantTreeData","DumpTreeData",&dump_data_imp_);

   ULong64_t entry = 0;
   std::cout<<"start sorting..."<<std::endl;
   while(tree_reader.Next()){
      if(!(entry%10000))
         std::cout<<"entry: "<<entry<<"/"<<total_entry<<" "<<(double)entry/(double)total_entry*100.<<"\%"<<std::endl;
      ++entry;
      PixTreeEvent* pixie_event = pixie_event_reader_.Get();
      processor_struct::PSPMT pspmt;
      if(!pixie_event->pspmt_vec_.empty())
          pspmt = pixie_event->pspmt_vec_.at(0);
      else
         continue;
  
      if(pspmt.xa_l>10 && pspmt.xb_l>10 && pspmt.ya_l>10 && pspmt.yb_l>10
            && pspmt.xa_l<30000 && pspmt.xb_l<30000 && pspmt.ya_l<30000
            && pspmt.yb_l<30000&&pspmt.dy_l>2000)
      {
         dump_data_imp_.Clear();
         dump_data_imp_.externalTS1 = pixie_event->externalTS1;
         dump_data_imp_.eventId = entry;
         dump_data_imp_.clover_vec_ = pixie_event->clover_vec_;
         dump_data_imp_.pspmt_vec_ = pixie_event->pspmt_vec_;
//         for(auto vandle : pixie_event->vandle_vec_){
//            if(vandle.tof>15||vandle.qdc>400) 
//               dump_data_imp_.vandle_vec_.emplace_back(vandle);
//         }
         implant_tree_->Fill();
      }
  
      if(pspmt.xa_h>10 && pspmt.xb_h>10 && pspmt.ya_h>10 && pspmt.yb_h>10
            && pspmt.xa_h<8100 && pspmt.xb_h<8100 && pspmt.ya_h<8100
            && pspmt.yb_h<8100&&pspmt.dy_h>10&&pspmt.dy_h<30000
            && pspmt.dy_l>-2000&&pspmt.dy_l<2000)
      {
         dump_data_beta_.Clear();
         dump_data_beta_.externalTS1 = pixie_event->externalTS1;
         dump_data_beta_.eventId = entry;
         dump_data_beta_.clover_vec_ = pixie_event->clover_vec_;
         dump_data_beta_.pspmt_vec_ = pixie_event->pspmt_vec_;
//         for(auto vandle : pixie_event->vandle_vec_){
//            if(vandle.tof>15||vandle.qdc>400) 
//               dump_data_beta_.vandle_vec_.emplace_back(vandle);
//         }
         beta_tree_->Fill();
      }
   }//end loop through the mergedData TTree

   beta_tree_->Write();
   implant_tree_->Write();
   output_file_.Close();
   file.Close();
   return 0;
}


