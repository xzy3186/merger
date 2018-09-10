#include <iostream>
#include "OutputTreeData.hpp"
#include "TreeDataLinkDef.h"
#include "YamlParameter.h"
#include "BetaTSScannor.hpp"
#include "BigRIPSTSScannor.hpp"
#include "ImplantTSScannor.hpp"
#include "ProcessorRootStruc.hpp"
#include "BigRIPSTreeData.h"

/** prints usage **/
void usage(char *argv0)
{
  std::cout << "[MergerMain]: Usage: "
      << argv0 << " -i [input_file_name] -c [configuration_file_name]"
      << " -o [output_file_name]" << std::endl;
}

int main(int argc, char **argv) 
{
    std::string input_file_name;
    std::string config_file_name;
    std::string output_file_name;
  
    /** parsing commandline arguments **/
    if(argc<3){
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
   
    /** timestamp scanors **/
    BigRIPSTSScannor brips_ts_scannor;
    BetaTSScannor beta_ts_scannor;
    ImplantTSScannor implant_ts_scannor;
    try {
	/** creates YamlParameter instance **/
        YamlParameter::Create(config_file_name);

	/** configures timestamp scannors with the yaml file **/
        brips_ts_scannor.Configure("BigRIPSTSScannor");
        beta_ts_scannor.Configure("BetaTSScannor");
        implant_ts_scannor.Configure("ImplantTSScannor");

	/** sets TTreeReaderValue objects **/
        brips_ts_scannor.SetReader();
        beta_ts_scannor.SetReader();
        implant_ts_scannor.SetReader();

	/** scans timestamps through the tree **/
        std::cout << "[MergerMain]: scanning BigRIPS events..." << std::endl;
        brips_ts_scannor.Scan();
        std::cout << "[MergerMain]: scanning Beta events..." << std::endl;
        beta_ts_scannor.Scan();
        std::cout << "[MergerMain]: scanning Implant events..." << std::endl;
        implant_ts_scannor.Scan();
    }
    catch (char* msg) {
        std::cout << msg << std::endl;
        return 1;
    }
    catch (std::bad_alloc){
        std::cout << "[MergerMain]: bad_alloc occured while setting up." << std::endl;
        return 1;
    }

    std::cout << "[MergerMain]: BigRIPS map size: " << brips_ts_scannor.GetMap().size() << std::endl;
    std::cout << "[MergerMain]: Beta map size: " << beta_ts_scannor.GetMap().size() << std::endl;
    std::cout << "[MergerMain]: Implant map size: " << implant_ts_scannor.GetMap().size() << std::endl;

    return 0;
}
