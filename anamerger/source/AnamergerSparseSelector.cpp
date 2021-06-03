#include "AnamergerSparseSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(AnamergerSparseSelector);

AnamergerSparseSelector::AnamergerSparseSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                                      beta_(tree_reader_, "mergedBeta"),
                                                                      clover_vec_(tree_reader_, "clover_vec_"),
                                                                      vandle_vec_(tree_reader_, "corrected_vandle_vec"),
                                                                      output_file_name_("anamerger_sparse_output.root") {
}

AnamergerSparseSelector::~AnamergerSparseSelector() {
    if (fHistArray) {
        delete fHistArray;
        fHistArray = nullptr;
    }
    if (fOutputFile) {
        delete fOutputFile;
        fOutputFile = nullptr;
    }
}

void AnamergerSparseSelector::Begin(TTree* mergedData) {
    GetOutputList()->Clear();
    if (fInput) {
        TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
        if (named)
            output_file_name_ = named->GetTitle();
    }
}

void AnamergerSparseSelector::SlaveBegin(TTree* mergedData) {
    if (fHistArray) {
        delete fHistArray;
        fHistArray = nullptr;
    }
    fHistArray = new TObjArray();

    fHistArray->Add(new THnSparseF("nQDC_nToF", "nQDC_nToF_Tib_clv_addback", 4, new Int_t[4]{3200, 1000, 1000, 8000},
                                   new Double_t[4]{-100, 0, -3, 0}, new Double_t[4]{1000, 32000, 3, 4000}));
    fHistArray->Add(new THnSparseF("YSO_pixels0_5", "YSO_pixels0_5", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels1_25", "YSO_pixels1_25", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels1_5", "YSO_pixels1_5", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels1_75", "YSO_pixels1_75", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels2", "YSO_pixels2", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels2_5", "YSO_pixels2_5", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels3", "YSO_pixels3", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels3_5", "YSO_pixels3_5", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("YSO_pixels4", "YSO_pixels4", 5, new Int_t[5]{2000, 200, 200, 200, 200}, new Double_t[5]{-3, 0, 0, 0, 0}, new Double_t[5]{3, 50, 50, 50, 50}));
    fHistArray->Add(new THnSparseF("Clover_Addback_Tib_TGammaBeta", "Clover_Addback_Tib_TGammaBeta", 3, new Int_t[3]{1000, 8000, 100}, new Double_t[3]{-3, 0, -1000}, new Double_t[3]{3, 4000, 500}));

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
    } else {
        std::cout << "SalveBegin() called. " << GetOutputList()->GetEntries() << " histograms are initialized." << std::endl;
    }

    return;
}

void AnamergerSparseSelector::Init(TTree* mergedData) {
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
        //if (beta->high_gain_.energy_sum_ < 200)
           // return kTRUE;
        for (const auto& imp : beta->output_vec_) {
            if (imp.output_vec_.empty())
                continue;
            if (imp.output_vec_.at(0).sts != 6)
                continue;
            const Double_t tib = (((double)beta->dyn_single_.time_ - (double)imp.dyn_single_.time_)) / 1.E+9;

            Double_t clover_addback_energy_1 = 0;
            Double_t tbeta_gamma_1 = 0;
            Double_t clover_addback_energy_2 = 0;
            Double_t tbeta_gamma_2 = 0;

            std::sort((*clover_vec_).begin(), (*clover_vec_).end(), [](const processor_struct::CLOVERS& a, const processor_struct::CLOVERS& b) -> bool { return a.energy < b.energy; });


          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<0.5)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels0_5");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }

          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<1.25)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels1_25");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }


          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<1.5)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels1_5");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }

          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<1.75)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels1_75");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }


          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<2.0)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels2");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }


          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<2.5)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels2_5");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }

          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<3.0)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels3");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }

          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<3.5)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels3_5");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }

          if (sqrt(TMath::Power((beta->high_gain_.id_x_-imp.low_gain_.id_x_),2) + TMath::Power((beta->high_gain_.id_y_-imp.low_gain_.id_y_),2))<=4.0)
           {
                 auto hist = (THnSparse *)fHistArray->FindObject("YSO_pixels4");
                 hist->Fill(new Double_t[5]{tib, beta->high_gain_.id_x_, beta->high_gain_.id_y_, imp.low_gain_.id_x_, imp.low_gain_.id_y_});
           }
            for (const auto& clv : *clover_vec) {
                if (clv.time - beta->dyn_single_.time_ > -850 && clv.time - beta->dyn_single_.time_ < -500) {
                    if (clv.detNum == 0 || clv.detNum == 1 || clv.detNum == 2 || clv.detNum == 3) {
                        clover_addback_energy_1 += clv.energy;
                        tbeta_gamma_1 = clv.time - beta->dyn_single_.time_;
                    }

                    if (clv.detNum == 4 || clv.detNum == 5 || clv.detNum == 6 || clv.detNum == 7) {
                        clover_addback_energy_2 += clv.energy;
                        tbeta_gamma_2 = clv.time - beta->dyn_single_.time_;
                    }
                }
            }

            if (clover_addback_energy_1 > 0) {
                auto hist = (THnSparse*)fHistArray->FindObject("Clover_Addback_Tib_TGammaBeta");
                hist->Fill(new Double_t[3]{tib, clover_addback_energy_1, tbeta_gamma_1});
            }

            if (clover_addback_energy_2 > 0) {
                auto hist = (THnSparse*)fHistArray->FindObject("Clover_Addback_Tib_TGammaBeta");
                hist->Fill(new Double_t[3]{tib, clover_addback_energy_2, tbeta_gamma_2});
            }

            for (auto& vandle : *vandle_vec) {
                const double tdiff_vb = (double)vandle.GetVandleData()->sTime - (double)beta->dyn_single_.time_;
                if (tdiff_vb < 200 || tdiff_vb > 250)
                    continue;
                //  if (tib > 0.01 && tib < time_window_) {
                {
                    if (clover_addback_energy_1 > 0) {
                        auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF");
                        hist->Fill(new Double_t[4]{vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc, tib, clover_addback_energy_1});
                    }

                    if (clover_addback_energy_2 > 0) {
                        auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF");
                        hist->Fill(new Double_t[4]{vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc, tib, clover_addback_energy_2});
                    }

                    /*if (clover_addback_energy_2 > 0) {
                        auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF");
                        hist->Fill(new Double_t[4]{vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc, tib, 0.0});
                    }*/
                }
                // }
                /***if (tib > (0.0 - time_window_) && tib < -0.01) {
					{
						auto hist = (THnSparse*)fHistArray->FindObject("nQDC_nToF_BG");
						hist->Fill(new Double_t[2]{ vandle.GetCorrectedToF(), vandle.GetVandleData()->qdc });
					}
				}****/
            }
        }

    }  //end loop through the mergedData TTree

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
    while (TObject* obj = next()) {
        std::cout << "[AnamergerSparseSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
        obj->Write();
    }

    fOutputFile->Close();

    return;
}
