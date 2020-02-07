#include <iostream>
#include <TFile.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include "TBetaEventList.h"
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"
#include "PaassRootStruct.hpp"
#include "BigRIPSTreeData.h"
#include "CorrectedVANDLEData.h"
#include "BetaEventSelector.h"

void usage(char const* arg)
{
	std::cout << arg << " -p [corrected_rootfiles_dir] -n [number_of_workers] -f [file_name] ";
	std::cout << "-t [time_window] -o [output_file_name]" << std::endl;
}

int main(int argc, char** argv) {
	std::string input_path;
	std::string file_name;
	std::string output_file;
	int n_workers = 1;
	double time_window;

	/** parsing commandline arguments **/
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}
	int opt = 0;
	while ((opt = getopt(argc, argv, "p:n:f:t:o:")) != -1) {
		switch (opt) {
		case 'p':
			input_path = optarg;
			break;
		case 'n':
			n_workers = std::stoi(optarg);
			break;
		case 'f':
			file_name = optarg;
			break;
		case 't':
			time_window = std::stod(optarg);
			break;
		case 'o':
			output_file = optarg;
			break;
		default:
			usage(argv[0]);
			return 1;
			break;
		}
	}

	TChain *chain = new TChain("mergedCorrectedBeta");
	for (int i = 0; i < n_workers; ++i) {
		std::ostringstream oss;
		oss << input_path << "0." << i << "/" << file_name;
		TChain tmp_chain("mergedCorrectedBeta");
		tmp_chain.Add(oss.str().c_str());
		if (!tmp_chain.GetEntries()) {
			std::cout << oss.str() << " has no entry. skipping..." << std::endl;
		}
		else {
			chain->Add(oss.str().c_str());
			std::cout << "Input file: " << oss.str() << " added to TChain." << std::endl;
		}
	}

	TBetaEventList* e = new TBetaEventList();
	TTreeReader tree_reader(chain);
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta(tree_reader,"mergedBeta");
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec(tree_reader,"corrected_vandle_vec");
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec(tree_reader,"clover_vec_");
	TTreeReaderValue <std::vector<processor_struct::GAMMASCINT>> gamma_scint_vec(tree_reader,"gamma_scint_vec_");

	std::cout << "starting an event loop " << std::endl;
	BetaEventSelector* selector = new BetaEventSelector();
	selector->SetEventList(e);
	selector->SetTimeWindow(time_window);
	chain->Process(selector);
	
	std::cout << e->GetMap().size() << " events have been added to the list." << std::endl;
	std::cout << "writing event list to a file: " << output_file << std::endl;
	TFile outputFile(output_file.c_str(),"update");
	e->Write(file_name.c_str());
	outputFile.Close();

	return 0;
}