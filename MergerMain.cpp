///@file MergerMain.cpp
///@brief beta-implant merger for E19044 experiment
///@author Rin Yokoyama
///@date October,15 2020

#include <iostream>
#include "OutputTreeData.hpp"
#include "TreeDataLinkDef.h"
#include "YamlParameter.hpp"
#include "E19044BetaTSScanor.hpp"
#include "E19044ImplantTSScanor.hpp"
#include "PaassRootStruct.hpp"
#include "OutputTreeData.hpp"
#include "TreeMerger.hpp"
#include "E19044BetaTreeMerger.hpp"
#include "PspmtAnalyzerData.hpp"
#include <TCutG.h>
#include <TFile.h>

/** prints usage **/
void usage(char *argv0)
{
  std::cout << "[MergerMain]: Usage: "
      << argv0 << "-c [configuration_file_name]"
      << std::endl;
}

int main(int argc, char **argv) 
{
    std::string input_file_name;
    std::string config_file_name;
    std::string output_file_name;
  
    /** parsing commandline arguments **/
    if(argc!=3){
      usage(argv[0]);
      return 1;
    }
    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:c:")) != -1) {
      switch (opt) {
        case 'i':
          input_file_name = optarg;
          break;
        case 'o':
          output_file_name = optarg;
          break;
        case 'c':
          config_file_name = optarg;
          break;
        default:
          usage(argv[0]);
          return 1;
          break;
      }
    }
   
    try {
	    /** creates YamlParameter instance **/
        YamlParameter::Create(config_file_name);
        YamlReader yaml_reader("MergerMain");

        
        std::string file_name = yaml_reader.GetString("TCutGFileName",false,"__no_pid_gate__");
        TFile* cut_file = nullptr;
        std::vector<TCutG*> pid_cuts;
        if (file_name != "__no_pid_gate__") {
            cut_file = new TFile(file_name.c_str());
            auto pid_vec = yaml_reader.GetStringVec("PIDCuts");
            for (const auto& pid : pid_vec) {
                auto cut = (TCutG*)cut_file->Get(pid.c_str());
                if (!cut) {
                    std::cout << "[MergerMain]: Cannot load TCutG named: " << pid << std::endl;
                    continue;
                }
                pid_cuts.emplace_back(cut);
            }
        }

        /* Scan beta events */
        E19044BetaTSScanor beta_ts_scannor;
	    /** configures timestamp scannors with the yaml file **/
        beta_ts_scannor.Configure("E19044BetaTSScannor");
	    /** sets TTreeReaderValue objects **/
        beta_ts_scannor.SetReader();
	     /** scans timestamps through the tree **/
        std::cout << "[MergerMain]: scanning Beta events..." << std::endl;
        beta_ts_scannor.Scan();

        std::cout << "[MergerMain]: Beta map size: " << beta_ts_scannor.GetIEntryMap().size() << std::endl;

        /** If no pid cuts specified, merge all events **/
        if (pid_cuts.empty()) {
            std::cout << "[MergerMain]: merging implant events to beta events..." << std::endl;

            /** timestamp scanors **/
            E19044ImplantTSScanor implant_ts_scanor;

	         /** configures timestamp scannors with the yaml file **/
            implant_ts_scanor.Configure("ImplantTSScannor");

	         /** sets TTreeReaderValue objects **/
            implant_ts_scanor.SetReader();

	         /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning Implant events..." << std::endl;
            implant_ts_scanor.Scan();

            std::cout << "[MergerMain]: Implant map size: " << implant_ts_scanor.GetIEntryMap().size() << std::endl;

	         /** runs merger **/
            E19044BetaTreeMerger beta_imp_merger(&beta_ts_scannor,&implant_ts_scanor);
            beta_imp_merger.Configure("E19044BetaTreeMerger");
            beta_imp_merger.Merge();
            beta_imp_merger.Write();

            std::cout << std::endl;
            std::cout << std::endl;

            return 0;
        }
        /* Loop over the vector of pid gate and merge for each cuts */
        for (auto pid : pid_cuts) {
            std::cout << "[MergerMain]: merging implant events for " << pid->GetName() << " to beta events..." << std::endl;

            /** timestamp scanors **/
            E19044ImplantTSScanor implant_ts_scanor;
            implant_ts_scanor.SetPidCut(pid);

	         /** configures timestamp scannors with the yaml file **/
            implant_ts_scanor.Configure("ImplantTSScannor");

	         /** sets TTreeReaderValue objects **/
            implant_ts_scanor.SetReader();

	         /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning Implant events..." << std::endl;
            implant_ts_scanor.Scan();

            std::cout << "[MergerMain]: Implant map size: " << implant_ts_scanor.GetIEntryMap().size() << std::endl;

	         /** runs merger **/
            E19044BetaTreeMerger beta_imp_merger(&beta_ts_scannor,&implant_ts_scanor);
            beta_imp_merger.SetFileSuffix(pid->GetName());
            beta_imp_merger.Configure("E19044BetaTreeMerger");
            beta_imp_merger.Merge();
            beta_imp_merger.Write();

            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
    catch (std::string msg) {
        std::cout << msg << std::endl;
        std::cout << "[MergerMain]: exiting from main() due to the error" << std::endl;
        return 1;
    }
    catch (std::bad_alloc){
        std::cout << "[MergerMain]: bad_alloc occured while setting up." << std::endl;
        std::cout << "[MergerMain]: exiting from main() due to the error" << std::endl;
        return 1;
    }

    return 0;
}
