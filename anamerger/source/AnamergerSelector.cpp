#include "AnamergerSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(AnamergerSelector);

AnamergerSelector::AnamergerSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                          beta_(tree_reader_, "mergedBeta"),
                                                          clover_vec_(tree_reader_, "clover_vec_"),
                                                          vandle_vec_(tree_reader_, "vandle_vec_"),
                                                          output_file_name_("anamerger_output_250ms.root") {
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
    fHistArray->Add(new TH2F("Tib_ClvE", "ClvE_Tib", 4000, 0, 4000, 1000, -4, 4));
    fHistArray->Add(new TH2F("Tib_HighE", "Tib_HighE", 4000, 0, 8000, 1000, -4, 4));
    fHistArray->Add(new TH2F("Tib_LowE", "Tib_LowE", 4000, 0, 8000, 1000, -3, 3));
    fHistArray->Add(new TH2F("nQDC_nToF", "nQDC_nToF", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nQDC_nToF_BG", "nQDC_nToF_BG", 1600, -100, 1500, 1000, 0, 32000));
    fHistArray->Add(new TH2F("nToF_nQDC", "nTof_nQDC", 20000, 0, 20000, 2200, -100, 1000));
    fHistArray->Add(new TH2F("nToF_nQDC2", "nTof_nQDC2", 20000, 0, 20000, 2200, -100, 1000));
    fHistArray->Add(new TH1F("nToF", "nTof", 1600, -100, 1500));
    fHistArray->Add(new TH1F("nToF_BG", "nTof_BG", 1600, -100, 1500));
    fHistArray->Add(new TH2F("BarN_nToF", "BarN_nToF", 1600, -100, 1500, 50, -0.5, 49.5));
    fHistArray->Add(new TH1F("Tib", "Tib", 1000, -4, 4));
    fHistArray->Add(new TH2F("clover_detNum", "clover_detNum", 12000, 0, 6000, 8, -1, 8));
    fHistArray->Add(new TH2F("YSO_image_rotated", "YSO_image_rotated", 2000, -10, 10, 2000, -10, 10));
    fHistArray->Add(new TH2F("YSO_image_rotated_beta", "YSO_image_rotated_beta", 2000, -10, 10, 2000, -10, 10));
    fHistArray->Add(new TH1F("clover_Energy", "clover_Energy", 6000, 0, 6000));
    //adding histograms to output list
    TIter next(fHistArray);
    while (auto hist = next()) {
        GetOutputList()->Add(hist);
    }
    n_correction = new TF1("n_correction", "[0]*pow(x,2)", 0, 25000);
    n_correction->SetParameters(0, 5.8742e-09);

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
        auto clover_vec = clover_vec_.Get();
        auto vandle_vec = vandle_vec_.Get();

        for (const auto& imp : beta->output_vec_) {
            if (imp.output_vec_.at(0).sts != 6 && beta->high_gain_.trace_energy_ > 380)
                continue;
            const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;
            ((TH1F*)fHistArray->FindObject("Tib"))->Fill(tib);
            ((TH2F*)fHistArray->FindObject("Tib_HighE"))->Fill(beta->high_gain_.energy_sum_, tib);

            if (imp.low_gain_.energy_sum_ > 500) {
                Double_t rotated_x = (imp.low_gain_.pos_x_);
                Double_t rotated_y = (imp.low_gain_.pos_y_);
                ((TH2F*)fHistArray->FindObject("YSO_image_rotated"))->Fill(rotated_x, rotated_y);
            }

            if (beta->high_gain_.energy_sum_ > 500) {
                Double_t rotated_x = (beta->high_gain_.pos_x_);
                Double_t rotated_y = (beta->high_gain_.pos_y_);
                ((TH2F*)fHistArray->FindObject("YSO_image_rotated_beta"))->Fill(rotated_x, rotated_y);
            }

            ((TH2F*)fHistArray->FindObject("Tib_LowE"))->Fill(beta->low_gain_.energy_, tib);
            for (const auto& clv : *clover_vec) {
                {
                    auto hist = (TH2F*)fHistArray->FindObject("Tib_ClvE");
                    hist->Fill(clv.energy, tib);
                }

                {
                    auto hist = (TH1F*)fHistArray->FindObject("clover_Energy");
                    hist->Fill(clv.energy);
                }

                {
                    auto hist = (TH2F*)fHistArray->FindObject("clover_detNum");
                    hist->Fill(clv.energy, clv.detNum);
                }
            }
            for (const auto& vandle : *vandle_vec) {
                if (tib > 0.001 && tib < 0.300 && beta->high_gain_.trace_energy_ > 380) {
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF");
                        hist->Fill(vandle.corTof - n_correction->Eval(vandle.qdc), vandle.qdc);
                    }

                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nToF_nQDC");
                        hist->Fill(vandle.qdc, vandle.corTof - n_correction->Eval(vandle.qdc));
                    }

                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nToF_nQDC2");
                        hist->Fill(vandle.qdc, vandle.corTof);
                    }

                    {
                        auto hist = (TH1F*)fHistArray->FindObject("nToF");
                        hist->Fill(vandle.corTof - n_correction->Eval(vandle.qdc));
                    }

                    {
                        auto hist = (TH2F*)fHistArray->FindObject("BarN_nToF");
                        hist->Fill(vandle.corTof, vandle.barNum);
                    }
                }
                if (tib > -0.300 && tib < -0.001 && beta->high_gain_.trace_energy_ > 380) {
                    {
                        auto hist = (TH2F*)fHistArray->FindObject("nQDC_nToF_BG");
                        hist->Fill(vandle.corTof - n_correction->Eval(vandle.qdc), vandle.qdc);
                    }
                    {
                        auto hist = (TH1F*)fHistArray->FindObject("nToF_BG");
                        hist->Fill(vandle.corTof - n_correction->Eval(vandle.qdc));
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
