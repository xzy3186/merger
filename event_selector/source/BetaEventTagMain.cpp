#include <iostream>
#include <TFile.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include "YamlParameter.hpp"
#include "TBetaEventList.h"
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"
#include "PaassRootStruct.hpp"
#include "BigRIPSTreeData.h"
#include "CorrectedVANDLEData.h"
#include "BetaEventTag.h"

void usage(char const* arg)
{
	std::cout << arg << " -i [input_rootfile] -c [config_file_name]" << std::endl;
}

int main(int argc, char** argv) {
	std::string input_file;
	std::string config_file;

	/** parsing commandline arguments **/
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}
	int opt = 0;
	while ((opt = getopt(argc, argv, "i:c:")) != -1) {
		switch (opt) {
		case 'i':
			input_file = optarg;
			break;
		case 'c':
			config_file = optarg;
			break;
		default:
			usage(argv[0]);
			return 1;
			break;
		}
	}

	YamlParameter::Create(config_file);
	BetaEventTag b_tag;
	YamlParameter::Destroy();

	TFile* file = new TFile(input_file.c_str(), "UPDATE");
	TTree* tree = (TTree*)file->Get("mergedCorrectedBeta");

	std::string nucl_name = input_file.substr(input_file.find("mergedCorrectedBeta")+20, 4);
	std::cout << "Nucl Name: " << nucl_name << std::endl;

	TTreeReader tree_reader(tree);
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta(tree_reader,"mergedBeta");
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec(tree_reader,"corrected_vandle_vec");
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec(tree_reader,"clover_vec_");
	TTreeReaderValue <std::vector<processor_struct::GAMMASCINT>> gamma_scint_vec(tree_reader,"gamma_scint_vec_");

	std::string tag;
	auto branch = tree->Branch("tag",&tag);
	Bool_t is_nucl_n = false;
	auto br2 = tree->Branch("IsNuclN", &is_nucl_n, "IsNuclN/O");

	std::cout << "starting an event loop " << std::endl;
	while (tree_reader.Next()) {
		auto b = beta.Get();
		tag = b_tag.GetTag(b->file_name_, b->event_number_);
		if (tag.find(nucl_name) != std::string::npos) {
			is_nucl_n = true;
		}
		else {
			is_nucl_n = false;
		}
		branch->Fill();
		br2->Fill();
	}
	
	tree->Write("",TObject::kOverwrite);
	file->Close();
	return 0;
}