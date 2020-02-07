#include "BetaEventSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(BetaEventSelector);

BetaEventSelector::BetaEventSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                          beta_(tree_reader_, "mergedBeta"),
                                                          clover_vec_(tree_reader_, "clover_vec_"),
                                                          vandle_vec_(tree_reader_, "corrected_vandle_vec"),
                                                          gamma_scint_vec_(tree_reader_, "gamma_scint_vec_") {
}

BetaEventSelector::~BetaEventSelector()
{
}

void BetaEventSelector::Begin(TTree* mergedData) {
    GetOutputList()->Clear();

}

void BetaEventSelector::SlaveBegin(TTree* mergedData)
{
	return;
}

void BetaEventSelector::Init(TTree* mergedData) {
	try {
		tree_reader_.SetTree(mergedData);
	}
	catch (...) {
		std::cout << "caught an error but resuming.." << std::endl;
	}
   return;
}

Bool_t BetaEventSelector::Process(Long64_t entry) {
	try {
		tree_reader_.SetLocalEntry(entry);
		{
			auto b = beta_.Get();
			if (!b)
				return kTRUE;
			auto vandles = vandle_vec_.Get();
			if (!vandles)
				return kTRUE;
			for (const auto& imp : b->output_vec_) {
				if (imp.output_vec_.empty())
					continue;
				if (imp.output_vec_.at(0).sts != 6)
					continue;
				const Double_t tib = (((double)b->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
				if (tib > 0.01 && tib < time_window_) {
					Bool_t banana = false;
					for (auto& vandle : *vandles) {
						const double tdiff_vb = (double)vandle.GetVandleData()->sTime - (double)b->dyn_single_.time_;
						if (tdiff_vb < 200 || tdiff_vb > 250)
							continue;
						if (vandle.GetCorrectedToF() > 25 && vandle.GetCorrectedToF() < 800) {
							banana = true;
						}
					}
					if (banana)
						fBetaEventList->AddEvent(b->file_name_, b->event_number_);
				}
			}
		}
	}
	catch (...) {
		std::cout << "caught an error but resuming.." << std::endl;
	}
	return kTRUE;
}

void BetaEventSelector::Terminate() {
   return;
}
