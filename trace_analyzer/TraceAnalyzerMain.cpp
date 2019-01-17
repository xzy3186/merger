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
#include "TraceAnalyzer.hpp"
#include "PspmtAnalyzer.hpp"
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
   PspmtAnalyzer pspmt_analyzer("PspmtAnalyzer");

   trace_analyzer.Begin();
   pspmt_analyzer.Begin();

   ULong64_t entry = 0;
   std::cout<<"start sorting..."<<std::endl;
   /* main event loop */
   while(tree_reader.Next()){
      if(!(entry%10000))
         std::cout<<"entry: "<<entry<<"/"<<total_entry<<" "<<(double)entry/(double)total_entry*100.<<"\%"<<std::endl;
      ++entry;
      PixTreeEvent* pixie_event = pixie_event_reader_.Get();
      if(pixie_event->pspmt_vec_.empty())
         continue;
      for(auto pspmt : pixie_event->pspmt_vec_){
         trace_analyzer.Process(pspmt, pixie_event->externalTS1);
      }
      pspmt_analyzer.Process(trace_analyzer.GetChannelVec()); 
      trace_analyzer.ClearVec();
   }//end loop through the TTree
   trace_analyzer.Terminate();
   pspmt_analyzer.Terminate();
   output_file_.Close();
   file.Close();
   return 0;
}


