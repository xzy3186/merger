#include "AnamergerSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(AnamergerSelector);

AnamergerSelector::AnamergerSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                          beta_(tree_reader_, "mergedBeta"),
                                                          clover_vec_(tree_reader_, "clover_vec_"),
                                                          vandle_vec_(tree_reader_, "corrected_vandle_vec"),
                                                          output_file_name_("anamerger_output.root") {
}

AnamergerSelector::~AnamergerSelector() {
}

void AnamergerSelector::Begin(TTree* mergedData) {
    GetOutputList()->Clear();
    if (fInput) {
        TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
        if (named)
            output_file_name_ = named->GetTitle();
    }
}

void AnamergerSelector::SlaveBegin(TTree* mergedData) {
    fHistArray = new TObjArray();

    fHistArray->Add(new TH2F("Tib_ClvE", "ClvE_Tib", 4000, 0, 4000, 1000, -3, 3));
    fHistArray->Add(new TH2F("Tib_HighE", "Tib_HighE", 4000, -1000, 18000, 1000, -3, 3));
    fHistArray->Add(new TH2F("Tib_LowE", "Tib_LowE", 4000, -1000, 18000, 1000, -3, 3));
    fHistArray->Add(new TH2F("Tib_DynE", "Tib_DynE", 4000, -1000, 18000, 1000, -3, 3));
    fHistArray->Add(new TH2F("Tib_Esum", "Tib_DynE", 4000, -1000, 18000, 1000, -3, 3));
    fHistArray->Add(new TH2F("nQDC_nToF", "nQDC_nToF", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_up_uncorr", "nQDC_nToF_up_uncorr", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_down_uncorr", "nQDC_nToF_down_uncorr", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_center", "nQDC_nToF_center", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_down", "nQDC_nToF_down", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_up", "nQDC_nToF_up", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_BG", "nQDC_nToF_BG", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nToF_nQDC", "nTof_nQDC", 1000, 0, 32000, 1100, -100, 1000));
    fHistArray->Add(new TH2F("BarN_nToF", "BarN_nToF", 1600, -100, 1500, 50, -0.5, 49.5));
    fHistArray->Add(new TH2F("BarNTdiff", "BarNTdiff", 8000, -100, 1500, 50, -0.5, 49.5));
    fHistArray->Add(new TH1F("Tib", "Tib", 1000, -3, 3));
    fHistArray->Add(new TH2F("Tib_nToF", "Tib_nToF", 1600, -100, 1500, 1000, -3, 3));
    fHistArray->Add(new TH1F("nMult", "nMult", 50, -0.5, 49.5));
    fHistArray->Add(new TH1F("nToF", "nToF", 2500, -500, 2000));
    fHistArray->Add(new TH2F("tofRatioVsQDC", "tofRatioVsQDC", 1000, 0, 2, 1000, 0, 32000));
    fHistArray->Add(new TH2F("BarN_Ratio", "BarN_Ratio", 1000, 0, 2, 50, -0.5, 49.5));
    fHistArray->Add(new TH2F("BarN_RatioUp", "BarN_RatioUp", 1000, 0, 2, 50, -0.5, 49.5));
    fHistArray->Add(new TH2F("BarN_RatioDown", "BarN_RatioDown", 1000, 0, 2, 50, -0.5, 49.5));
    fHistArray->Add(new TH2F("BarN_RatioLeft", "BarN_RatioLeft", 1000, 0, 2, 50, -0.5, 49.5));
    fHistArray->Add(new TH2F("BarN_RatioRight", "BarN_RatioRight", 1000, 0, 2, 50, -0.5, 49.5));
    //adding histograms to output list
    TIter next(fHistArray);
    while (auto hist = next()) {
        GetOutputList()->Add(hist);
    }

    n_correction = new TF1("n_correction", "[0]+[1]*pow(x,2)", 0, 65536);
    n_correction->SetParameters(0, 6.5E-9);

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
    } else {
        std::cout << "SalveBegin() called. " << GetOutputList()->GetEntries() << " histograms are initialized." << std::endl;
    }

    return;
}

void AnamergerSelector::Init(TTree* mergedData) {
    tree_reader_.SetTree(mergedData);
    return;
}

Bool_t AnamergerSelector::Process(Long64_t entry) {
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
        //if (beta->low_gain_.energy_ > 2000)
        //	return kTRUE;
        for (const auto& imp : beta->output_vec_) {
            if (imp.output_vec_.empty())
                continue;
            if (imp.output_vec_.at(0).sts != 6)
                continue;
            const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
            ((TH1F*)fHistArray->FindObject("Tib"))->Fill(tib);
            ((TH2F*)fHistArray->FindObject("Tib_HighE"))->Fill(beta->high_gain_.energy_sum_, tib);
            ((TH2F*)fHistArray->FindObject("Tib_LowE"))->Fill(beta->low_gain_.energy_, tib);
            ((TH2F*)fHistArray->FindObject("Tib_DynE"))->Fill(beta->dyn_single_.energy_, tib);
            ((TH2F*)fHistArray->FindObject("Tib_Esum"))->Fill(beta->high_gain_.energy_sum_, tib);
            for (const auto& clv : *clover_vec) {
                {
                    auto hist = (TH2F*)fHistArray->FindObject("Tib_ClvE");
                    hist->Fill(clv.energy, tib);
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
                {
                    auto hist = (TH2F*)fHistArray->FindObject("Tib_nToF");
                    hist->Fill(vandle.GetCorrectedToF(), tib);
                }
                if (tib > 0.01 && tib < time_window_) {
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF");
                        hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                    }
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("tofRatioVsQDC");
                        hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                    }

                    {
                        auto hist = (TH2F*)fHistArray->FindObject("BarN_Ratio");
                        hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                    }
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("BarNTdiff");
                        hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) - (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                    }

                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nToF_nQDC");
                        hist->Fill(vandle.GetVandleData()->qdc, vandle.GetCorrectedToF());
                    }
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("BarN_nToF");
                        hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                    }

                    {
                        auto hist = (TH1F*)fHistArray->FindObject("nToF");
                        hist->Fill(vandle.GetCorrectedToF());
                    }
                    if (vandle.GetTransformedY() > 2.0) {
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_up");
                            hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                        }
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_up_uncorr");
                            hist->Fill(vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc), vandle.GetVandleData()->qdc);
                        }
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("BarN_RatioUp");
                            hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                        }
                    }
                    if (vandle.GetTransformedX() < -2.0) {
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("BarN_RatioLeft");
                            hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                        }
                    }
                    if (vandle.GetTransformedX() > 2.0) {
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("BarN_RatioRight");
                            hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                        }
                    }
                    if (vandle.GetTransformedY() < -2.0) {
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_down");
                            hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                        }

                        {
                            auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_down_uncorr");
                            hist->Fill(vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc), vandle.GetVandleData()->qdc);
                        }
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("BarN_RatioDown");
                            hist->Fill((vandle.GetVandleData()->corTof - n_correction->Eval(vandle.GetVandleData()->qdc)) / (double)vandle.GetCorrectedToF(), vandle.GetVandleData()->barNum);
                        }
                    }
                    if (vandle.GetTransformedX() > -2.0 && vandle.GetTransformedX() < 2.0) {
                        {
                            auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_center");
                            hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                        }
                    }
                }
                if (tib > (0.0 - time_window_) && tib < -0.01) {
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_BG");
                        hist->Fill(vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc);
                    }
                }
            }
        }

    }  //end loop through the mergedData TTree

    return kTRUE;
}

void AnamergerSelector::Terminate() {
    fOutputFile = new TFile(output_file_name_.c_str(), "recreate");
    std::cout << "[AnamergerSelector::Terminate()]: output file: " << output_file_name_ << std::endl;
    // write the histograms
    TIter next(GetOutputList());
    while (TObject* obj = next()) {
        std::cout << "[AnamergerSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
        obj->Write();
    }

    fOutputFile->Close();

    return;
}
