///@file NextMergerMain.cpp
///@brief Merger for the E19044 NEXT and VANDLE data
///@author Rin Yokoyama
///@date October,16 2020

#include <iostream>
#include "TreeDataLinkDef.h"
#include "YamlParameter.hpp"
#include "E19044NEXTTSScanor.hpp"
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
            std::cout << "[MergerMain]: Merging NEXT events to VANDLE events..." << std::endl;

            /** timestamp scanors **/
            E19044NEXTTSScanor next_ts_scannor;
            E19044MergedVANDLETSScanor vandle_ts_scannor;

	         /** configures timestamp scannors with the yaml file **/
            next_ts_scannor.Configure("E19044NEXTTSScannor");
            vandle_ts_scannor.Configure("E19044VANDLETSScannor");

	         /** sets TTreeReaderValue objects **/
            next_ts_scannor.SetReader();
            vandle_ts_scannor.SetReader();

	         /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning NEXT events..." << std::endl;
            next_ts_scannor.Scan();
            std::cout << "[MergerMain]: scanning VANDLE events..." << std::endl;
            vandle_ts_scannor.Scan();

            std::cout << "[MergerMain]: NEXT map size: " << next_ts_scannor.GetIEntryMap().size() << std::endl;
            std::cout << "[MergerMain]: VANDLE map size: " << vandle_ts_scannor.GetIEntryMap().size() << std::endl;

	         /** runs merger **/
            TreeMerger<NEXTOutput, E19044Output, PixTreeEvent> next_merger(&vandle_ts_scannor, &next_ts_scannor);
            next_merger.Configure("NEXTMerger");
            next_merger.Merge();
            next_merger.Write();

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
