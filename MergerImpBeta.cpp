#include <iostream>
#include "OutputTreeData.hpp"
#include "TreeDataLinkDef.h"
#include "YamlParameter.hpp"
#include "BetaTSScannor.hpp"
#include "BigRIPSTSScannor.hpp"
#include "ImplantTSScannor.hpp"
#include "ProcessorRootStruc.hpp"
#include "BigRIPSTreeData.h"
#include "OutputTreeData.hpp"
#include "BetaTreeMerger.hpp"
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
            MergedImplantTSScannor mimp_ts_scannor;
            BetaTSScannor beta_ts_scannor;

	    /** configures timestamp scannors with the yaml file **/
            mimp_ts_scannor.Configure("MergedImplantTSScannor");
            beta_ts_scannor.Configure("BetaTSScannor");

	    /** sets TTreeReaderValue objects **/
            mimp_ts_scannor.SetReader();
            beta_ts_scannor.SetReader();

	    /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning MergedImplant events..." << std::endl;
            mimp_ts_scannor.Scan();
            std::cout << "[MergerMain]: scanning Beta events..." << std::endl;
            beta_ts_scannor.Scan();

            std::cout << "[MergerMain]: MergedImplant map size: " << mimp_ts_scannor.GetMap().size() << std::endl;
            std::cout << "[MergerMain]: Beta map size: " << beta_ts_scannor.GetMap().size() << std::endl;

    	    /** runs merger **/
            BetaTreeMerger<OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>, PspmtData, OutputTreeData<PspmtData, TreeData>> imp_beta_merger(&beta_ts_scannor,&mimp_ts_scannor);
            imp_beta_merger.Configure("ImplantBetaMerger");
            imp_beta_merger.Merge();
            imp_beta_merger.Write();
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
