#include <iostream>
#include <fstream>
#include <string>
#include "TChain.h"
#include "TProof.h"
#include "TROOT.h"
#include "YamlParameter.h"
#include "YamlReader.hpp"
#include "AnamergerPidSelector.h"

void usage(char const *arg)
{
  std::cout << arg << "-c [yaml_file]" << std::endl;
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

  //gROOT->SetBatch(true); 

  try {
    /** creates YamlParameter instance **/
    YamlParameter::Create(config_file_name);
    YamlReader *yaml_reader_ = new YamlReader("AnamergerMain");
    const std::string tree_name = yaml_reader_->GetString("TreeName");
    const std::string merger_list_name = yaml_reader_->GetString("MergerListName");
    const std::string num_workers = yaml_reader_->GetString("NumWorkers");

    const std::string proof_arg = "workers=" + num_workers;
    TProof *pr = TProof::Open("localhost",proof_arg.c_str());

    TChain *chain = new TChain(tree_name.c_str());
    std::ifstream mergerListFiles( merger_list_name.c_str() );
    std::string tempName("");
    
    while ( getline (mergerListFiles,tempName) ) 
    {
      chain->AddFile( tempName.c_str() );
      std::cout << tempName.c_str() << std::endl;
    }
    
    mergerListFiles.close();
    std::cout << "Number of TTrees added to the chain: " << chain->GetNtrees() << std::endl;
  
    const unsigned long long n_entries = yaml_reader_->GetULong64("NumEntries",false,chain->GetEntries());
    const unsigned long long first_entry = yaml_reader_->GetULong64("FirstEntry",false,0);

    //chain->SetProof();
    //std::cout << "SetProof to the chain: " << chain->GetName() << std::endl;

    AnamergerPidSelector *selector = new AnamergerPidSelector(chain);

    std::cout << "Start Processing..." << std::endl;
    chain->Process(selector,"",n_entries, first_entry);
    //pr->Process(tree_name.c_str(),selector,"",n_entries, first_entry);

    pr->Close();

    /** destroys YamlParameter instance **/
      YamlParameter::Destroy();
  }
  catch (std::string msg) {
      std::cout << msg << std::endl;
      std::cout << "[AnamergerMergerMain]: exiting from main() due to the error" << std::endl;
      return 1;
  }
  catch (std::bad_alloc){
      std::cout << "[AnamergerMergerMain]: bad_alloc occured while setting up." << std::endl;
      std::cout << "[AnamergerMergerMain]: exiting from main() due to the error" << std::endl;
      return 1;
  }
 
  return 0;
}