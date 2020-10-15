///@file MergerMain.cpp
///@brief beta-implant merger for E19044 experiment
///@author Rin Yokoyama
///@date October,15 2020

#include <iostream>
#include "OutputTreeData.hpp"
#include "TreeDataLinkDef.h"
#include "YamlParameter.hpp"
#include "E19044BetaTSScanor.hpp"
#include "ImplantTSScannor.hpp"
#include "PaassRootStruct.hpp"
#include "OutputTreeData.hpp"
#include "TreeMerger.hpp"
#include "E19044BetaTreeMerger.hpp"
#include "PspmtAnalyzerData.hpp"

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

        /** merges implant events to beta events **/
        {
            std::cout << "[MergerMain]: merging implant events to beta events..." << std::endl;

            /** timestamp scanors **/
            E19044BetaTSScanor beta_ts_scannor;
            ImplantTSScannor implant_ts_scannor;

	         /** configures timestamp scannors with the yaml file **/
            beta_ts_scannor.Configure("E19044BetaTSScannor");
            implant_ts_scannor.Configure("ImplantTSScannor");

	         /** sets TTreeReaderValue objects **/
            beta_ts_scannor.SetReader();
            implant_ts_scannor.SetReader();

	         /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning Beta events..." << std::endl;
            beta_ts_scannor.Scan();
            std::cout << "[MergerMain]: scanning Implant events..." << std::endl;
            implant_ts_scannor.Scan();

            std::cout << "[MergerMain]: Beta map size: " << beta_ts_scannor.GetIEntryMap().size() << std::endl;
            std::cout << "[MergerMain]: Implant map size: " << implant_ts_scannor.GetIEntryMap().size() << std::endl;

	         /** runs merger **/
            E19044BetaTreeMerger beta_imp_merger(&implant_ts_scannor,&beta_ts_scannor);
            beta_imp_merger.Configure("BetaImplantMerger");
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
