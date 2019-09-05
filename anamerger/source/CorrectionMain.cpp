#include <iostream>
#include <fstream>
#include <string>
#include "TChain.h"
#include "TProof.h"
#include "TROOT.h"
#include "YamlParameter.hpp"
#include "YamlReader.hpp"
#include "CorrectionSelector.h"
#include "LibraryConfig.h"

void usage(char const* arg)
{
	std::cout << arg << "-c [yaml_file] -o [output_file]" << std::endl;
}

int main(int argc, char** argv)
{
	std::string input_file_name;
	std::string config_file_name;
	std::string output_file_name = "correction_output.root";

	/** parsing commandline arguments **/
	if (argc < 3) {
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

	gROOT->SetBatch();
	TProof* pr;

	try {
		/** creates YamlParameter instance **/
		YamlParameter::Create(config_file_name);
		YamlReader* yaml_reader_ = new YamlReader("AnamergerMain");
		const std::string tree_name = yaml_reader_->GetString("TreeName");
		const std::string merger_list_name = yaml_reader_->GetString("MergerListName");
		const Double_t time_window = yaml_reader_->GetDouble("TimeWindow");
		const bool use_proof = yaml_reader_->GetBoolean("UseProof", false, false);
		if (use_proof) {
			const std::string num_workers = yaml_reader_->GetString("NumWorkers");
			const std::string proof_arg = "workers=" + num_workers;
			pr = TProof::Open("lite://", proof_arg.c_str());
			//auto lib_path = pr->Getenv("LD_LIBRARY_PATH");
			//std::string new_path = lib_path.Data();
			//new_path = new_path + ":" + getYamlcppLibDir() + ":" + getMergerLibDir() + ":" + getPaassLibDir();
			//std::string new_path = "/opt/root/6.18.0/lib/:" + getYamlcppLibDir() + ":" + getMergerLibDir() + ":" + getPaassLibDir();
			//pr->AddEnvVar("LD_LIBRARY_PATH", new_path.c_str());
			//std::cout << "LD_LIBRARY_PATH=" << pr->Getenv("LD_LIBRARY_PATH").Data() << std::endl;
			//std::cout << "NEW_PATH=" << new_path << std::endl;
			//pr->AddEnvVar("LD_LIBRARY_PATH", new_path.c_str());
			std::vector<std::string> libs =
			{
			  getYamlcppLibDir() + "/libyaml-cpp.so",
			  getMergerLibDir() + "/libTraceAnalyzerLib.so",
			  getMergerLibDir() + "/libmerger_data_dic.so",
			  getMergerLibDir() + "/libMergerLib.so",
			  getMergerLibDir() + "/libAnamergerLib.so",
			  getMergerLibDir() + "/libCorrectionLib.so",
			  getPaassLibDir() + "/lib/libSysRootStrucLib.so",
			};
			for (const auto& lib : libs) {
				pr->Load(lib.c_str());
				//std::string cmd = "gSystem->Load(\\\"" + lib + "\\\");";
				//std::cout << "gProof->Exec(" << cmd << ")" << std::endl;
				//gProof->Exec(cmd.c_str());
			}
		}
		TChain* chain = new TChain(tree_name.c_str());
		std::ifstream mergerListFiles(merger_list_name.c_str());
		std::string tempName("");

		while (getline(mergerListFiles, tempName))
		{
			chain->AddFile(tempName.c_str());
			std::cout << tempName.c_str() << std::endl;
		}

		mergerListFiles.close();
		std::cout << "[CorrectionMain]: Number of TTrees added to the chain: " << chain->GetNtrees() << std::endl;

		const unsigned long long n_entries = yaml_reader_->GetULong64("NumEntries", false, chain->GetEntries());
		const unsigned long long first_entry = yaml_reader_->GetULong64("FirstEntry", false, 0);
		const std::string proof_output_location = yaml_reader_->GetString("ProofOutputLocation", false, "./");

		if (use_proof) {
			chain->SetProof();
			std::cout << "SetProof to the chain: " << chain->GetName() << std::endl;
			pr->AddInput(new TParameter<Bool_t>("use_proof", true));
			pr->AddInput(new TNamed("output_file_prefix", output_file_name.c_str()));
			pr->AddInput(new TNamed("proof_output_location", proof_output_location.c_str()));
			chain->Process("CorrectionSelector", "", n_entries, first_entry);
		}
		else {
			std::cout << "Start Processing (Proof OFF)..." << std::endl;
			CorrectionSelector* selector = new CorrectionSelector(chain);
			selector->SetFileName(output_file_name);
			chain->Process(selector, "", n_entries, first_entry);
		}

		if (use_proof)
			pr->Close();

		/** destroys YamlParameter instance **/
		YamlParameter::Destroy();
	}
	catch (std::string msg) {
		std::cout << msg << std::endl;
		std::cout << "[CorrectionMain]: exiting from main() due to the error" << std::endl;
		return 1;
	}
	catch (std::bad_alloc) {
		std::cout << "[CorrectionMain]: bad_alloc occured while setting up." << std::endl;
		std::cout << "[CorrectionMain]: exiting from main() due to the error" << std::endl;
		return 1;
	}

	return 0;
}
