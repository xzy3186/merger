#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <map>
#include <dirent.h>
#include "TFile.h"
#include "TH1.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TSelector.h"
#include "Rtypes.h"
#include "TMath.h"
#include "PaassRootStruct.hpp"
#include "TraceAnalyzer.hpp"
#include "PspmtAnalyzer.hpp"
#include "YamlParameter.hpp"
#include "YamlReader.hpp"

int main(int argc, char **argv){
   /* print usage */
   if(argc!=4){
      std::cout << "Usage: trace_analyzer_main [input file] [output file] [config file]"
         << std::endl;
      std::cout << "When TChainFlag in the config file is false." << std::endl;
      std::cout << "Usage: trace_analyzer_main [input file prefix] [output file] [config file]"
         << std::endl;
      std::cout << "When TChainFlag in the config file is true." << std::endl;
      return 1;
   }

   /* Open config file */
   YamlParameter::Create(argv[3]);
   YamlReader yaml_reader("RunSetting");
   const bool chain_flag = yaml_reader.GetBoolean("TChainFlag",false,0); /*use TChain or not*/
   const ULong64_t print_freq = yaml_reader.GetULong64("PrintFrequency",false,10000); /*cout frequency*/

   /* create TChain and add input files */
   TChain *chain = new TChain("PixTree");
   if(chain_flag){ /* add all files matches argv[1]*.root */
      const std::string fileprefix(argv[1]);
      const std::size_t found = fileprefix.find_last_of("/\\");
      const std::string file_path = fileprefix.substr(0,found);
      const std::string file_prefix = fileprefix.substr(found+1);
      std::cout << "path: " << file_path << "file prefix: " << file_prefix << std::endl;
      struct dirent **namelist = NULL;
      const int dir_elements = scandir(file_path.c_str(),&namelist,NULL,NULL);
      std::vector<std::string> input_files;
      for(int i=0; i<dir_elements; ++i){
         const std::string element_name(namelist[i]->d_name);
         if( element_name.find(file_prefix) != std::string::npos 
            && element_name.find(".root") != std::string::npos){
            
            input_files.push_back(file_path + "/" + element_name);
         }
      }
      sort(input_files.begin(),input_files.end());
      for(auto input_file : input_files){
         chain->Add(input_file.c_str());
         std::cout << "file added: " << input_file << std::endl;
      }
   }
   else{ /* add single file when TChainFlag is false */
      std::cout<<"input file: "<<argv[1]<<std::endl;
      chain->Add(argv[1]);
   }
   ULong64_t total_entry = chain->GetEntries();
   std::cout<<"input tree entries: "<<total_entry<<std::endl;

   /* Set TTreeReader */
   TTreeReader tree_reader(chain);
   TTreeReaderValue<PixTreeEvent> pixie_event_reader_(tree_reader, "PixTreeEvent");

   /* Open output file */
   std::cout<<"output file: "<<argv[2]<<std::endl;
   TFile output_file_(argv[2],"RECREATE");

   const uint64_t initial_event = yaml_reader.GetULong64("InitialEvent",false,0);
   const uint64_t num_events = yaml_reader.GetULong64("NumEvents",false,
                                                      tree_reader.GetEntries(kTRUE));

   PspmtAnalyzer pspmt_analyzer("PspmtAnalyzer");

   pspmt_analyzer.Begin();

   ULong64_t entry = initial_event;
   if(entry)
      tree_reader.SetEntry(entry); /* sets first entry if its not zero */

   std::cout<<"start sorting from " << initial_event << " to " << num_events
      << "/" << entry << std::endl;

   /* main event loop */
   while( tree_reader.Next() && num_events > (entry-initial_event) ){
      if(!(entry%print_freq)) /*prints progress in every print_freq events*/
         std::cout<<"entry: "<<entry<<"/"<<total_entry<<" "
            <<(double)entry/(double)total_entry*100.<<"\%"<<std::endl;

      ++entry;
      PixTreeEvent* pixie_event = pixie_event_reader_.Get();
      if(pixie_event->pspmt_vec_.empty()) /* skips if there is no pspmt event */
         continue;
      pspmt_analyzer.SetEventId(pixie_event->fileName, pixie_event->eventNum);
      pspmt_analyzer.SetEventData(pixie_event);
      pspmt_analyzer.Process(pixie_event->pspmt_vec_, pixie_event->externalTS1);
   }//end loop through the TTree

   pspmt_analyzer.Terminate();
   output_file_.Close();
   return 0;
}
