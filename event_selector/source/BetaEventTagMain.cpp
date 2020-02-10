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

	TTreeReader tree_reader(tree);
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta(tree_reader,"mergedBeta");
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec(tree_reader,"corrected_vandle_vec");
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec(tree_reader,"clover_vec_");
	TTreeReaderValue <std::vector<processor_struct::GAMMASCINT>> gamma_scint_vec(tree_reader,"gamma_scint_vec_");

	std::string tag;
	auto branch = tree->Branch("tag",&tag);

	std::cout << "starting an event loop " << std::endl;
	while (tree_reader.Next()) {
		auto b = beta.Get();
		tag = b_tag.GetTag(b->file_name_, b->event_number_);
		branch->Fill();
	}
	
	tree->Write();
	file->Close();
	return 0;
}