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
#include "TraceAnalyzerData.hpp"
#include "TraceAnalyzer.hpp"
#include "YamlParameter.hpp"


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

   //TraceAnalyzerData trace_analyzer_data;    
   //TTree *trave_tree_ = new TTree("trace_tree","trace_tree");
   //beta_tree_->Branch("TraceAnalyzerData","TraceAnalyzerData",&trace_analyzer_data);
  
   YamlParameter::Create(argv[3]);
   TraceAnalyzer trace_analyzer("TraceAnalyzer");
   ULong64_t entry = 0;
   std::cout<<"start sorting..."<<std::endl;
   while(tree_reader.Next()){
      if(!(entry%10000))
         std::cout<<"entry: "<<entry<<"/"<<total_entry<<" "<<(double)entry/(double)total_entry*100.<<"\%"<<std::endl;
      ++entry;
      PixTreeEvent* pixie_event = pixie_event_reader_.Get();
      for(auto pspmt : pixie_event->pspmt_vec_){
         trace_analyzer.Process(pspmt);
      }
   }//end loop through the mergedData TTree

   //trace_tree_->Write();
   output_file_.Close();
   file.Close();
   return 0;
}


