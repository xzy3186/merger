#include "AnamergerSparseSelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerSparseSelector);

AnamergerSparseSelector::AnamergerSparseSelector(TTree* mergedData) :
	tree_reader_(mergedData),
	beta_(tree_reader_, "mergedBeta"),
	clover_vec_(tree_reader_, "clover_vec_"),
	vandle_vec_(tree_reader_, "corrected_vandle_vec"),
	output_file_name_("anamerger_sparse_output.root")
{

}

AnamergerSparseSelector::~AnamergerSparseSelector()
{
	if (fHistArray) {
		delete fHistArray;
		fHistArray = nullptr;
	}
	if (fOutputFile) {
		delete fOutputFile;
		fOutputFile = nullptr;
	}
}

void AnamergerSparseSelector::Begin(TTree* mergedData)
{
	GetOutputList()->Clear();
	if (fInput) {
		TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
		if (named)
			output_file_name_ = named->GetTitle();
	}
}

void AnamergerSparseSelector::SlaveBegin(TTree* mergedData)
{
	if (fHistArray) {
		delete fHistArray;
		fHistArray = nullptr;
	}
	fHistArray = new TObjArray();

	fHistArray->Add(new THnSparseD("nQDC_nToF", "nQDC_nToF", 2, new Int_t[2]{ 3200,1000 },
		new Double_t[2]{ -100, 0 }, new Double_t[2]{ 1000, 32000 }));

	//adding histograms to output list
	TIter next(fHistArray);
	while (auto hist = next()) {
		GetOutputList()->Add(hist);
	}

	if (fInput) {
		TParameter<Double_t>* time_window = (TParameter<Double_t>*)fInput->FindObject("TimeWindow");
		if (time_window)
			time_window_ = time_window->GetVal();
		else
			time_window_ = 1.0;
	}

	if (gProofServ) {
		const TString msg = TString::Format("SlaveBegin() of Ord = %s called. %d histograms are initialized.",
			gProofServ->GetOrdinal(), GetOutputList()->GetEntries());
		gProofServ->SendAsynMessage(msg);
	}
	else {
		std::cout << "SalveBegin() called. " << GetOutputList()->GetEntries() << " histograms are initialized." << std::endl;
	}

	return;
}

void AnamergerSparseSelector::Init(TTree* mergedData)
{
	tree_reader_.SetTree(mergedData);
	return;
}

Bool_t AnamergerSparseSelector::Process(Long64_t entry) {
	tree_reader_.SetLocalEntry(entry);
	{
		auto beta = beta_.Get();
		if (!beta)
			return kTRUE;
		auto clover_vec = clover_vec_.Get();
		if (!clover_vec)
			return kTRUE;
		auto vandle_vec = vandle_vec_.Get();
		if (!vandle_vec)
			return kTRUE;
		if (beta->high_gain_.energy_sum_ < 200)
			return kTRUE;
		for (const auto& imp : beta->output_vec_) {
			if (imp.output_vec_.empty())
				continue;
			if( imp.output_vec_.at(0).sts != 6 )
				continue;
			const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
			for (auto& vandle : *vandle_vec) {
				const double tdiff_vb = (double)vandle.GetVandleData()->sTime - (double)beta->dyn_single_.time_;
				if (tdiff_vb < 200 || tdiff_vb > 250)
					continue;
				if (tib > 0.01 && tib < time_window_) {
					{
						auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF");
						hist->Fill(new Double_t[2]{ vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc });
					}
				}
				if (tib > (0.0 - time_window_) && tib < -0.01) {
					{
						auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF_BG");
						hist->Fill(new Double_t[2]{ vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc });
					}
				}
			}
		}

	}//end loop through the mergedData TTree

	return kTRUE;
}

void AnamergerSparseSelector::Terminate() {

	if (fOutputFile) {
		delete fOutputFile;
		fOutputFile = nullptr;
	}
	fOutputFile = new TFile(output_file_name_.c_str(), "recreate");
	std::cout << "[AnamergerSparseSelector::Terminate()]: output file: " << output_file_name_ << std::endl;
	// write the histograms
	TIter next(GetOutputList());
	while (TObject * obj = next()) {
		std::cout << "[AnamergerSparseSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
		obj->Write();
	}

	fOutputFile->Close();

	return;
}
