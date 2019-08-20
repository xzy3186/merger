#include "AnamergerSelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerSelector);

AnamergerSelector::AnamergerSelector(TTree* mergedData) :
	tree_reader_(mergedData),
	beta_(tree_reader_, "mergedBeta"),
	clover_vec_(tree_reader_, "clover_vec_"),
	vandle_vec_(tree_reader_, "vandle_vec_"),
	output_file_name_("anamerger_output.root")
{

}

AnamergerSelector::~AnamergerSelector()
{

}

void AnamergerSelector::Begin(TTree* mergedData)
{
	GetOutputList()->Clear();
	if (fInput) {
		TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
		if (named)
			output_file_name_ = named->GetTitle();
	}
}

void AnamergerSelector::SlaveBegin(TTree* mergedData)
{
	fHistArray = new TObjArray();

	fHistArray->Add(new TH2F("Tib_ClvE", "ClvE_Tib", 4000, 0, 4000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_HighE", "Tib_HighE", 4000, 0, 8000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_LowE", "Tib_LowE", 4000, 0, 8000, 1000, -3, 3));
	fHistArray->Add(new TH2F("nQDC_nToF", "nQDC_nToF", 1600, -100, 1500, 1000, 0, 32000));
	fHistArray->Add(new TH2F("nQDC_nToF_BG", "nQDC_nToF_BG", 1600, -100, 1500, 1000, 0, 32000));
	fHistArray->Add(new TH2F("nToF_nQDC", "nTof_nQDC", 1000, 0, 32000, 1600, -100, 1000));
	fHistArray->Add(new TH2F("BarN_nToF", "BarN_nToF", 1600, -100, 1500, 50, -0.5, 49.5));
	fHistArray->Add(new TH1F("Tib", "Tib", 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_nToF", "Tib_nToF", 1600, -100, 1500, 1000, -3, 3));
	fHistArray->Add(new TH1F("nMult", "nMult", 50, -0.5, 49.5));
	fHistArray->Add(new TH1F("nToF", "nToF", 2500, -500, 2000));

	//adding histograms to output list
	TIter next(fHistArray);
	while (auto hist = next()) {
		GetOutputList()->Add(hist);
	}

	n_correction = new TF1("n_correction", "[0]+[1]*pow(x,2)", 0, 65536);
	n_correction->SetParameters(2.9, 6.5E-9);

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

void AnamergerSelector::Init(TTree* mergedData)
{
	tree_reader_.SetTree(mergedData);
	return;
}

Bool_t AnamergerSelector::Process(Long64_t entry) {

	tree_reader_.SetLocalEntry(entry);
	{
		auto beta = beta_.Get();
		auto clover_vec = clover_vec_.Get();
		auto vandle_vec = vandle_vec_.Get();
		for (const auto& imp : beta->output_vec_) {
			if (imp.output_vec_.empty())
				continue;
			if( imp.output_vec_.at(0).sts != 6 )
				continue;
			const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
			((TH1F*)fHistArray->FindObject("Tib"))->Fill(tib);
			((TH2F*)fHistArray->FindObject("Tib_HighE"))->Fill(beta->high_gain_.energy_sum_, tib);
			((TH2F*)fHistArray->FindObject("Tib_LowE"))->Fill(beta->low_gain_.energy_, tib);
			for (const auto& clv : *clover_vec) {
				auto hist = (TH2F*)fHistArray->FindObject("Tib_ClvE");
				hist->Fill(clv.energy, tib);
			}
			{
				auto hist = (TH1F*)fHistArray->FindObject("nMult");
				hist->Fill(vandle_vec->size());
			}
			for (const auto& vandle : *vandle_vec) {
				{
					auto hist = (TH2F*)fHistArray->FindObject("Tib_nToF");
					hist->Fill(vandle.tof,tib);
				}
				if (tib > 0.01 && tib < time_window_) {
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF");
						hist->Fill(vandle.tof - n_correction->Eval(vandle.qdc), vandle.qdc);
					}
					{
						auto hist = (TH2F*)fHistArray->FindObject("nToF_nQDC");
						hist->Fill(vandle.qdc, vandle.tof);
					}
					{
						auto hist = (TH2F*)fHistArray->FindObject("BarN_nToF");
						hist->Fill(vandle.tof, vandle.barNum);
					}
					{
						auto hist = (TH1F*)fHistArray->FindObject("nToF");
						hist->Fill(vandle.tof);
					}
				}
				if (tib > (0.0 - time_window_) && tib < -0.01) {
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_BG");
						hist->Fill(vandle.tof - n_correction->Eval(vandle.qdc), vandle.qdc);
					}
				}
			}
		}

	}//end loop through the mergedData TTree

	return kTRUE;
}

void AnamergerSelector::Terminate() {

	fOutputFile = new TFile(output_file_name_.c_str(), "recreate");
	std::cout << "[AnamergerSelector::Terminate()]: output file: " << output_file_name_ << std::endl;
	// write the histograms
	TIter next(GetOutputList());
	while (TObject * obj = next()) {
		std::cout << "[AnamergerSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
		obj->Write();
	}

	fOutputFile->Close();

	return;
}
