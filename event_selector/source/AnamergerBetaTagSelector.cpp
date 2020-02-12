#include "AnamergerBetaTagSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(AnamergerBetaTagSelector);

AnamergerBetaTagSelector::AnamergerBetaTagSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                          beta_(tree_reader_, "mergedBeta"),
                                                          clover_vec_(tree_reader_, "clover_vec_"),
                                                          vandle_vec_(tree_reader_, "corrected_vandle_vec"),
                                                          gamma_scint_vec_(tree_reader_, "gamma_scint_vec_"),
																			 tag_(tree_reader_, "tag"),
																			 is_nucl_n_(tree_reader_,"IsNuclN"),
                                                          output_file_name_("anamerger_output.root") {
}

AnamergerBetaTagSelector::~AnamergerBetaTagSelector()
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

void AnamergerBetaTagSelector::Begin(TTree* mergedData) {
    GetOutputList()->Clear();
    if (fInput) {
        TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
        if (named)
            output_file_name_ = named->GetTitle();
    }
}

void AnamergerBetaTagSelector::SlaveBegin(TTree* mergedData)
{
	if (fHistArray) {
		delete fHistArray;
		fHistArray = nullptr;
	}
	fHistArray = new TObjArray();

	fHistArray->Add(new TH1F("Tib", "Tib", 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_ClvE", "ClvE_Tib", 4000, 0, 4000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_HagDynE", "HagDynE_Tib", 4000, 0, 4000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_HagAnE", "HagAnE_Tib", 4000, 0, 4000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_HighE", "Tib_HighE", 4000, -1000, 18000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_LowE", "Tib_LowE", 4000, -1000, 18000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_DynE", "Tib_DynE", 4000, -1000, 18000, 1000, -3, 3));
	fHistArray->Add(new TH2F("Tib_Esum", "Tib_Esum", 4000, -1000, 18000, 1000, -3, 3));
	fHistArray->Add(new TH2F("nQDC_nToF", "nQDC_nToF", 3200, -100, 1500, 8000, 0, 32000));
	fHistArray->Add(new TH2F("nQDC_nEnergy", "nQDC_nEnergy", 4000, 0, 10, 8000, 0, 32000));
	fHistArray->Add(new TH2F("nQDC_nToF_BG", "nQDC_nToF_BG", 3200, -100, 1500, 8000, 0, 32000));
	fHistArray->Add(new TH2F("nQDC_nEnergy_BG", "nQDC_nEnergy_BG", 4000, 0, 10, 8000, 0, 32000));
	fHistArray->Add(new TH2F("ClvE_nToF", "ClvE_nToF", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH2F("ClvE_nToF_BG", "ClvE_nToF_BG", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH2F("HagDynE_nToF", "HagDynE_nToF", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH2F("HagDynE_nToF_BG", "HagDynE_nToF_BG", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH2F("HagAnE_nToF", "HagAnE_nToF", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH2F("HagAnE_nToF_BG", "HagAnE_nToF_BG", 3200, -100, 1500, 2000, 0, 4000));
	fHistArray->Add(new TH1F("nMult", "nMult", 50, -0.5, 49.5));

	//adding histograms to output list
	TIter next(fHistArray);
	while (auto hist = next()) {
		GetOutputList()->Add(hist);
	}

	n_correction = new TF1("n_correction", "[0]+[1]*pow(x,2)", 0, 65536);
	n_correction->SetParameters(0.0, 6.5E-9);

	n_banana_up = new TF1("banana_up", "[0]/(x-[1])^[2]+[3]", 35, 1000);
	n_banana_up->SetParameters(170000, 35, 1.2, 400);
	n_banana_low = new TF1("banana_low", "[0]/(x-[1])^[2]+[3]", 25, 1000);
	n_banana_low->SetParameters(170000, 25, 1.3, -500);

	t_entry_ = 0;

	if (fInput) {
		TParameter<Double_t>* time_window = (TParameter<Double_t>*)fInput->FindObject("TimeWindow");
		if (time_window)
			time_window_ = time_window->GetVal();
		else
			time_window_ = 1.0;

      TNamed* named = (TNamed*)fInput->FindObject("nucl_name");
      if (named)
          nucl_name_ = named->GetTitle();
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

void AnamergerBetaTagSelector::Init(TTree* mergedData) {
    tree_reader_.SetTree(mergedData);
    return;
}

Bool_t AnamergerBetaTagSelector::Process(Long64_t entry) {
	t_entry_++;
	tree_reader_.SetLocalEntry(entry);
	{
		auto beta = beta_.Get();
		if (!beta)
			return kTRUE;
		if ((*tag_) != "" && (*tag_) != nucl_name_)
			return kTRUE;

		auto clover_vec = clover_vec_.Get();
		if (!clover_vec)
			return kTRUE;
		auto vandle_vec = vandle_vec_.Get();
		if (!vandle_vec)
			return kTRUE;
		auto gamma_scint_vec = gamma_scint_vec_.Get();
		if (!gamma_scint_vec)
			return kTRUE;
		if (beta->dyn_single_.energy_ > 20000)
			return kTRUE;
		if (beta->f11_left_.energy_ > 0 && beta->f11_right_.energy_ > 0)
			return kTRUE;

		for (const auto& imp : beta->output_vec_) {
			if (imp.output_vec_.empty())
				continue;
			if( imp.output_vec_.at(0).sts != 6 )
				continue;
			const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
			((TH1F*)fHistArray->FindObject("Tib"))->Fill(tib);
			((TH2F*)fHistArray->FindObject("Tib_HighE"))->Fill(beta->high_gain_.energy_sum_, tib);
			((TH2F*)fHistArray->FindObject("Tib_LowE"))->Fill(beta->low_gain_.energy_, tib);
			((TH2F*)fHistArray->FindObject("Tib_DynE"))->Fill(beta->dyn_single_.energy_, tib);
			((TH2F*)fHistArray->FindObject("Tib_Esum"))->Fill(beta->high_gain_.energy_sum_, tib);

			Double_t clover1 = 0;
			Double_t clover2 = 0;
			for (const auto& clv : *clover_vec) {
				const Double_t bg_tdiff = (beta->dyn_single_.time_ - clv.time);
				if (bg_tdiff < -1000 || bg_tdiff > 1000)
					continue;
				if (clv.detNum < 4)
					clover1 += clv.energy;
				else
					clover2 += clv.energy;
			}
			std::vector<double> clv_hits;
			if (clover1 > 0)
				clv_hits.push_back(clover1);
			if (clover2 > 0)
				clv_hits.push_back(clover2);
			for(const auto &hit : clv_hits)
			{
				auto hist = (TH2F*)fHistArray->FindObject("Tib_ClvE");
				hist->Fill(hit, tib);
			}


			for (const auto& hag : *gamma_scint_vec) {
				const Double_t bg_tdiff = (beta->dyn_single_.time_ - hag.time);
				if (bg_tdiff < -1000 || bg_tdiff > 1000)
					continue;
				if (hag.detNum % 2) {
					auto hist = (TH2F*)fHistArray->FindObject("Tib_HagAnE");
					hist->Fill(hag.energy, tib);
				}
				else {
					auto hist = (TH2F*)fHistArray->FindObject("Tib_HagDynE");
					hist->Fill(hag.energy, tib);

				}
			}
			{
				auto hist = (TH1F*)fHistArray->FindObject("nMult");
				hist->Fill(vandle_vec->size());
			}
			for (auto& vandle : *vandle_vec) {
				const double tdiff_vb = (double)vandle.GetVandleData()->sTime - (double)beta->dyn_single_.time_;
				if (tdiff_vb < 200 || tdiff_vb > 250)
					continue;

				if (tib > 0.01 && tib < time_window_) {
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF");
						hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
					}
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nEnergy");
						hist->Fill(vandle.GetNeutronEnergy(), vandle.GetVandleData()->qdc);
					}
					for (const auto& hit : clv_hits) {
						auto hist = (TH2F*)fHistArray->FindObject("ClvE_nToF");
						hist->Fill(vandle.GetCorrectedToF(), hit);
					}

					for (const auto& hag : *gamma_scint_vec) {
						if (hag.detNum % 2) {
							auto hist = (TH2F*)fHistArray->FindObject("HagAnE_nToF");
							hist->Fill(vandle.GetCorrectedToF(), hag.energy);
						}
						else {
							auto hist = (TH2F*)fHistArray->FindObject("HagDynE_nToF");
							hist->Fill(vandle.GetCorrectedToF(), hag.energy);
						}
					}
				}
				if (tib > (0.0 - time_window_) && tib < -0.01) {
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_BG");
						hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
					}
					{
						auto hist = (TH2F*)fHistArray->FindObject("nQDC_nEnergy_BG");
						hist->Fill(vandle.GetNeutronEnergy(), vandle.GetVandleData()->qdc);
					}

					for (const auto& hit : clv_hits) {
						auto hist = (TH2F*)fHistArray->FindObject("ClvE_nToF_BG");
						hist->Fill(vandle.GetCorrectedToF(), hit);
					}

					for (const auto& hag : *gamma_scint_vec) {
						if (hag.detNum % 2) {
							auto hist = (TH2F*)fHistArray->FindObject("HagAnE_nToF_BG");
							hist->Fill(vandle.GetCorrectedToF(), hag.energy);
						}
						else {
							auto hist = (TH2F*)fHistArray->FindObject("HagDynE_nToF_BG");
							hist->Fill(vandle.GetCorrectedToF(), hag.energy);
						}
					}
				}
			}
		}

	}//end loop through the mergedData TTree

	return kTRUE;
}

void AnamergerBetaTagSelector::Terminate() {
    fOutputFile = new TFile(output_file_name_.c_str(), "recreate");
    std::cout << "[AnamergerBetaTagSelector::Terminate()]: output file: " << output_file_name_ << std::endl;
    // write the histograms
    TIter next(GetOutputList());
    while (TObject* obj = next()) {
        std::cout << "[AnamergerBetaTagSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
        obj->Write();
    }

	fOutputFile->Close();

	if (fOutputFile) {
		delete fOutputFile;
		fOutputFile = nullptr;
	}

    return;
}

double AnamergerBetaTagSelector::CalculateNEnergy(const double& tof) const {
	const double v = 1.05 / (tof * 1.e-9);
	const double c = 299792458;
	const double beta = v * v / (c * c);
	const double gamma = 1. / sqrt(1. - beta);
	const double mass = 939.5654133;
	return mass * (gamma - 1.);
}
