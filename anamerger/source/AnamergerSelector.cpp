#include "AnamergerSelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerSelector);
  
AnamergerSelector::AnamergerSelector(TTree* mergedData):
   tree_reader_ (mergedData),
   beta_  (tree_reader_, "mergedBeta"),
   clover_vec_  (tree_reader_, "clover_vec_"),
   vandle_vec_  (tree_reader_, "vandle_vec_")
{

}

AnamergerSelector::~AnamergerSelector()
{

}

void AnamergerSelector::Begin(TTree* mergedData)
{
  GetOutputList()->Clear();
}

void AnamergerSelector::SlaveBegin(TTree* mergedData)
{
  fHistArray = new TObjArray();

  fHistArray->Add(new TH2F("Tib_ClvE","ClvE_Tib",4000,0,4000,1000,-3,3));
  fHistArray->Add(new TH2F("Tib_HighE","Tib_HighE",4000,0,8000,1000,-3,3));
  fHistArray->Add(new TH2F("Tib_LowE","Tib_LowE",4000,0,8000,1000,-3,3));
  fHistArray->Add(new TH2F("nQDC_nToF","nQDC_nToF",1000,-100,500,1000,0,8000));
  fHistArray->Add(new TH1F("Tib","Tib",1000,-3,3));

  //adding histograms to output list
  TIter next(fHistArray);
  while( auto hist = next() ){
     GetOutputList()->Add(hist);
  }

  if (gProofServ) {
    const TString msg = TString::Format("SlaveBegin() of Ord = %s called. %d histograms are initialized.",
                                         gProofServ->GetOrdinal(),GetOutputList()->GetEntries());
    gProofServ->SendAsynMessage(msg);
  }
  else {
    std::cout << "SalveBegin() called. " << GetOutputList()->GetEntries() << " histograms are initialized." << std::endl;
  }

  return;
}

void AnamergerSelector::Init(TTree* mergedData)
{    
  tree_reader_.SetTree( mergedData );
  return;
}

Bool_t AnamergerSelector::Process(Long64_t entry){

  tree_reader_.SetLocalEntry(entry);
  {
    auto beta = beta_.Get();
    auto clover_vec = clover_vec_.Get();
    auto vandle_vec = vandle_vec_.Get();
    for( const auto &imp : beta->output_vec_){
      const Double_t tib = (40.*((double)beta->external_ts_high_ - (double)imp.external_ts_low_))/1.E+9;
      ((TH1F*)fHistArray->FindObject("Tib"))->Fill(tib);
      ((TH2F*)fHistArray->FindObject("Tib_HighE"))->Fill(beta->high_gain_.energy_sum_,tib);
      ((TH2F*)fHistArray->FindObject("Tib_LowE"))->Fill(beta->low_gain_.energy_,tib);
      for( const auto &clv : *clover_vec){
        auto hist = (TH2F*)fHistArray->FindObject("Tib_ClvE");
        hist->Fill(clv.energy,tib);
      }
      for( const auto &vandle : *vandle_vec){
        auto hist = (TH2F*)fHistArray->FindObject("nQDC_nTOF");
        hist->Fill(vandle.tof,vandle.qdc);
      }
    }

  }//end loop through the mergedData TTree

  return kTRUE;
}

void AnamergerSelector::Terminate(){
 
  {
    const std::string fname = "anamerger_output.root";
    fOutputFile = new TFile(fname.c_str(),"recreate");
  }

  // write the histograms
  TIter next(GetOutputList());
  while( TObject* obj = next() ){
    std::cout << "[AnamergerSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
    obj->Write();
  }

  fOutputFile->Close();
  fHistArray->Clear();
  
  return ;
}
