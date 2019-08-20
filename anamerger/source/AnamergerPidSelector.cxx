// #################################################################################################################
// 
// # Alvaro Tolosa Delgado @ IFIC(Valencia,Spain)  alvaro.tolosa@ific.uv.es
// # Copyright (c) 2016 Jorge Agramunt & Alvaro Tolosa. All rights reserved.
// 
// #################################################################################################################

#include "AnamergerPidSelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerPidSelector)
  
AnamergerPidSelector::AnamergerPidSelector(TTree* merged_data):
   tree_reader_(merged_data),
   merged_event_(tree_reader_, "mergedImp")
{

}

AnamergerPidSelector::~AnamergerPidSelector()
{

}

void AnamergerPidSelector::Begin(TTree* merged_data)
{
  GetOutputList()->Clear();
}

void AnamergerPidSelector::SlaveBegin(TTree* merged_data)
{

  YamlReader yaml_reader("AnamergerPidSelector"); 
  {
    const std::string cut_file_name = yaml_reader.GetString("ReferenceCutFile");
    LoadCUTG(cut_file_name);
  }
/*  
  {
    const std::string yso_map_name = yaml_reader.GetString("YSOMap")
    fYSOMap = new YSOMap(ANAMERGER_PATH + "yso_map.txt");
  }
 */ 

  hist_array_ = new TObjArray();

  hist_array_->Add(new TH1F("tof_tot","tof_tot",10000,-1000,1000));

  //adding histograms to output list
  TIter next(hist_array_);
  while( TObject* obj = next() ){
     GetOutputList()->Add(obj);
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

void AnamergerPidSelector::Init(TTree* merged_data)
{    
  //tree_reader_.SetTree( merged_data );
  return;
}


Bool_t AnamergerPidSelector::Process(Long64_t entry){

  tree_reader_.SetLocalEntry(entry);
  {
    OutputTreeData<PixTreeEvent,TreeData> *imp = merged_event_.Get();

    for(auto isotope : vectorIsotopes){
      // PID gate
      if(imp->output_vec_.size()!=1)
        continue;
      if(isotope.IsInside(imp->output_vec_.at(0).aoq, imp->output_vec_.at(0).zet)){
        // PSPMT dynode L
        bool is_implant = false;
        for(auto pspmt : imp->pspmt_vec_){
          const std::string hname = "pspmt_dy_l_"+isotope.isotopeName;
          ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(pspmt.dy_l);
          if(pspmt.dy_l>10&&pspmt.dy_l<16000)
            is_implant = true;
        }
        if(is_implant){
          const std::string hname = "XYprofile_"+isotope.isotopeName;
          for(auto pspmt : imp->pspmt_vec_){
              ((TH2F*)GetOutputList()->FindObject(hname.c_str()))->Fill(pspmt.xposL,pspmt.yposL);
          }
        }
        { // count YSO implantation
          const std::string hname = "implant_"+isotope.isotopeName;
          ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(0);
          if(is_implant)
            ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(1);
        }
        { // isomer gamma with clovers
          for(auto clover : imp->clover_vec_){
            if(imp->pspmt_vec_.size()==1){
              const Double_t tdiff = clover.Time - imp->pspmt_vec_.at(0).dyL_time;
              const std::string hname = "isomer_clover_"+isotope.isotopeName;
              ((TH2F*)GetOutputList()->FindObject(hname.c_str()))->Fill(clover.Energy,tdiff);
            }
          }
        }
      }
    }
  }//end loop through the merged_data TTree

  return kTRUE;
}

void AnamergerPidSelector::Terminate(){

  YamlReader yaml_reader("AnamergerPidSelector"); 
  {
    const std::string output_file_name = yaml_reader.GetString("OutputFileName");
    output_file_ = new TFile(output_file_name.c_str(),"recreate");
  }
  // write the histograms
  TIter next(GetOutputList());
  while( TObject* obj = next() ){
    obj->Write();
    std::cout << "object: " << obj->GetName() << " has been written." << std::endl;
  }

  output_file_->Close();
  //hist_array_->Clear();
  
  return ;
}

int AnamergerPidSelector::LoadCUTG(const std::string &icutname)
{
  if(icutname.find(".root")==std::string::npos){
    std::ifstream fcut(icutname);
    if(!fcut){
      std::cerr << "File " << icutname.c_str() << " not found. Selector empty.\n";
      return 1;
    }
    while(!fcut.eof()){
      std::string isoname;
      fcut >> isoname;
      if(!isoname.length())
        break;
      Double_t ellipse_a, ellipse_b, ellipse_x0, ellipse_y0;
      fcut >> ellipse_a;
      fcut >> ellipse_b;
      fcut >> ellipse_x0;
      fcut >> ellipse_y0;
      std::cout << "Adding " << isoname << " as an isotope." << std::endl;
      vectorIsotopes.push_back( hIsotope(isoname, ellipse_a, ellipse_b, ellipse_x0, ellipse_y0, GetOutputList()) );
    }
    fcut.close();
  }
  else{
    TFile * const fcut = new TFile(icutname.c_str());
     
    if(!fcut){
      std::cerr << "File " << icutname.c_str() << " not found. Selector empty.\n";
      return 1;
    }
    TKey *key = 0;
    TIter keyNext( fcut->GetListOfKeys());
    TCutG * temp = nullptr;
    while ((key = (TKey *) keyNext())) {
      std::string tempName( key->GetName() );

      if (tempName.find("CUTG") != std::string::npos || tempName.find("cutg") != std::string::npos)
      {
       fcut->GetObject(key->GetName(), temp );

       vectorIsotopes.push_back ( hIsotope( temp, GetOutputList() ) );
      }
    }
    fcut->Close(); 
    delete fcut;
  }
  return 0;

}
 
