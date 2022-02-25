///@file NextMergerMain.cpp
///@brief Merger for the E19044 NEXT and VANDLE data
///@author Rin Yokoyama
///@date October,16 2020

#include <iostream>
#include "TreeDataLinkDef.h"
#include "YamlParameter.hpp"
#include "FDSiVANDLETSScanor.hpp"
#include "PaassRootStruct.hpp"
#include "TreeMerger.hpp"

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
            std::cout << "[FDSiTestMergerMain]: Merging two crates for test..." << std::endl;

            /** timestamp scanors **/
            FDSiVANDLETSScanor vandle_ts_scannor;
            FDSiVANDLETSScanor fdsi_ts_scannor;

	         /** configures timestamp scannors with the yaml file **/
            fdsi_ts_scannor.Configure("FDSiFDSiTSScannor");
            vandle_ts_scannor.Configure("FDSiVandleTSScannor");

	         /** sets TTreeReaderValue objects **/
            fdsi_ts_scannor.SetReader();
            vandle_ts_scannor.SetReader();

	         /** scans timestamps through the tree **/
            std::cout << "[FDSiVANDLEMergerMain]: scanning events from FDSi crate..." << std::endl;
            fdsi_ts_scannor.Scan();
            std::cout << "[FDSiVANDLEMergerMain]: scanning events from VANDLE crate..." << std::endl;
            vandle_ts_scannor.Scan();

            std::cout << "[FDSiVANDLEMergerMain]: FDSi crate map size: " << fdsi_ts_scannor.GetIEntryMap().size() << std::endl;
            std::cout << "[FDSiVANDLEMergerMain]: VANDLE crate map size: " << vandle_ts_scannor.GetIEntryMap().size() << std::endl;

	         /** runs merger **/
            TreeMerger<FDSiTestOutput, PixTreeEvent, PixTreeEvent> test_merger(&vandle_ts_scannor, &fdsi_ts_scannor);
            test_merger.Configure("FDSiTestMerger");
            test_merger.Merge();
            test_merger.Write();

            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
    catch (std::string msg) {
        std::cout << msg << std::endl;
        std::cout << "[FDSiTestMergerMain]: exiting from main() due to the error" << std::endl;
        return 1;
    }
    catch (std::bad_alloc){
        std::cout << "[FDSiTestMergerMain]: bad_alloc occured while setting up." << std::endl;
        std::cout << "[FDSiTestMergerMain]: exiting from main() due to the error" << std::endl;
        return 1;
    }

    return 0;
}
