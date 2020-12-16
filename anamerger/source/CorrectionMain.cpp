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
		const std::string vandle_corrector_config = yaml_reader_->GetString("VANDLECorrectorConfig");
		const Double_t time_window = yaml_reader_->GetDouble("TimeWindow", false, 0);
		const bool use_proof = yaml_reader_->GetBoolean("UseProof", false, false);
		if (use_proof) {
			const std::string num_workers = yaml_reader_->GetString("NumWorkers");
			const std::string proof_arg = "workers=" + num_workers;
			pr = TProof::Open("lite://", proof_arg.c_str());
			std::vector<std::string> libs =
			{
			  getYamlcppLibDir() + "/libyaml-cpp.so",
			  getMergerLibDir() + "libTraceAnalyzerLib.so",
			  getMergerLibDir() + "libmerger_data_dic.so",
			  getMergerLibDir() + "libMergerLib.so",
			  getMergerLibDir() + "libAnamergerLib.so",
			  getMergerLibDir() + "libCorrectionLib.so",
			  getPaassLibDir() + "/lib/libPaassRootStruct.so",
			};
			for (const auto& lib : libs) {
				std::cout << "loading libraries to proofserv: " << lib << std::endl;
				pr->Load(lib.c_str());
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
		std::vector<std::string> output_branches = yaml_reader_->GetStringVec("OutputBranches", false, *(new std::vector<std::string>));

		/** destroys YamlParameter instance **/
		YamlParameter::Destroy();

		if (use_proof) {
			chain->SetProof();
			std::cout << "SetProof to the chain: " << chain->GetName() << std::endl;
			pr->AddInput(new TParameter<Bool_t>("use_proof", true));
			pr->AddInput(new TNamed("output_file_prefix", output_file_name.c_str()));
			pr->AddInput(new TNamed("proof_output_location", proof_output_location.c_str()));
			pr->AddInput(new TNamed("vandle_corrector_config", vandle_corrector_config.c_str()));
			auto list = new TList();
			list->SetName("output_branches");
			for (const auto& br_name : output_branches) {
				list->Add(new TObjString(br_name.c_str()));
			}
			pr->AddInput(list);
			chain->Process("CorrectionSelector", "", n_entries, first_entry);
		}
		else {
			std::cout << "Start Processing (Proof OFF)..." << std::endl;
			CorrectionSelector* selector = new CorrectionSelector(chain);
			selector->SetFileName(output_file_name);
			selector->SetCorrectorConfigName(vandle_corrector_config);
			selector->SetOutputBranches(output_branches);
			chain->Process(selector, "", n_entries, first_entry);
		}

		if (use_proof)
			pr->Close();

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
