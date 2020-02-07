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
		chain->Add(oss.str().c_str());
		std::cout << "Input file: " << oss.str() << " added to TChain." << std::endl;
	}

	TBetaEventList e;
	TTreeReader tree_reader(chain);
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta(tree_reader,"mergedBeta");
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec(tree_reader,"corrected_vandle_vec");

	std::cout << "starting an event loop " << std::endl;
	while (tree_reader.Next()) {
		auto b = beta.Get();
		if (!b)
			continue;
		auto vandles = vandle_vec.Get();
		if (!vandles)
			continue;
		for (const auto& imp : b->output_vec_) {
			if (imp.output_vec_.empty())
				continue;
			if (imp.output_vec_.at(0).sts != 6)
				continue;
			const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
			if (tib > 0.01 && tib < time_window) {
				Bool_t banana = false;
				for (auto& vandle : *vandles) {
					const double tdiff_vb = (double)vandle.GetVandleData()->sTime - (double)b->dyn_single_.time_;
					if (tdiff_vb < 200 || tdiff_vb > 250)
						continue;
					if (vandle.GetCorrectedToF() > 25 && vandle.GetCorrectedToF() < 800 ) {
						banana = true;
					}
				}
				if(banana)
					e.AddEvent(b->file_name_,b->event_number_);
			}
		}
	}
	
	std::cout << "writing event list to a file: " << output_file << std::endl;
	TFile outputFile(output_file.c_str(),"update");
	e.Write(file_name.c_str());
	outputFile.Close();

	return 0;
}