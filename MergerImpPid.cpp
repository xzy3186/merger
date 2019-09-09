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
#include "PspmtData.hpp"
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

        /** merges BigRIPS events to implant events **/
        {
            std::cout << "[MergerMain]: merging BigRIPS events to implants events..." << std::endl;

            /** timestamp scanors **/
            BigRIPSTSScannor brips_ts_scannor;
            ImplantTSScannor implant_ts_scannor;

	    /** configures timestamp scannors with the yaml file **/
            brips_ts_scannor.Configure("BigRIPSTSScannor");
            implant_ts_scannor.Configure("ImplantTSScannor");

	    /** sets TTreeReaderValue objects **/
            brips_ts_scannor.SetReader();
            implant_ts_scannor.SetReader();

	    /** scans timestamps through the tree **/
            std::cout << "[MergerMain]: scanning BigRIPS events..." << std::endl;
            brips_ts_scannor.Scan();
            std::cout << "[MergerMain]: scanning Implant events..." << std::endl;
            implant_ts_scannor.Scan();

            std::cout << "[MergerMain]: BigRIPS map size: " << brips_ts_scannor.GetMap().size() << std::endl;
            std::cout << "[MergerMain]: Implant map size: " << implant_ts_scannor.GetMap().size() << std::endl;

	    /** runs merger **/
            TreeMerger<OutputTreeData<PspmtData, TreeData>, PspmtData, TreeData> brips_imp_merger(&implant_ts_scannor,&brips_ts_scannor);
            brips_imp_merger.Configure("BigRIPSImplantMerger");
            brips_imp_merger.Merge();
            brips_imp_merger.Write();

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
