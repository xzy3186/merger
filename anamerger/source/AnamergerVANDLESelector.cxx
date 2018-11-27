// #################################################################################################################
// 
// # Alvaro Tolosa Delgado @ IFIC(Valencia,Spain)  alvaro.tolosa@ific.uv.es
// # Copyright (c) 2016 Jorge Agramunt & Alvaro Tolosa. All rights reserved.
// 
// #################################################################################################################

#include "AnamergerVANDLESelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerVANDLESelector)

   AnamergerVANDLESelector::AnamergerVANDLESelector(TTree* merged_data):
      tree_reader_(merged_data),
      merged_event_(tree_reader_, "mergedImpBeta")
{

}

AnamergerVANDLESelector::~AnamergerVANDLESelector()
{

}

void AnamergerVANDLESelector::Begin(TTree* merged_data)
{
   GetOutputList()->Clear();
}

void AnamergerVANDLESelector::SlaveBegin(TTree* merged_data)
{

   YamlReader yaml_reader("AnamergerVANDLESelector"); 
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

void AnamergerVANDLESelector::Init(TTree* merged_data)
{    
   //tree_reader_.SetTree( merged_data );
   return;
}


Bool_t AnamergerVANDLESelector::Process(Long64_t entry){

   tree_reader_.SetLocalEntry(entry);
   {
      //OutputTreeData<PixTreeEvent,TreeData> *imp = merged_event_.Get();
      OutputTreeData<OutputTreeData<PixTreeEvent,TreeData>, PixTreeEvent> *impbeta = merged_event_.Get();
      auto imp_vec = impbeta->OutputTreeData<PixTreeEvent,TreeData>::output_vec_;
      auto beta_vec = impbeta->output_vec_;

      for(auto isotope : vectorIsotopes){
         // PID gate
         //if(imp->output_vec_.size()!=1)
         if(imp_vec.size()!=1)
            continue;
         if(isotope.IsInside(imp_vec.at(0).aoq, imp_vec.at(0).zet)){
            for(auto beta : beta_vec){
               for(auto vandle : beta.vandle_vec_){
                  if(vandle.corTof>=20 && vandle.corTof<=520){
                     //for TOF spectrum
                     const std::string hname = "vandle_corTof_"+isotope.isotopeName;
                     ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(vandle.corTof);
                  }
                  {
                     //for the QDC position
                     const std::string hname = "vandle_qdcPos_"+isotope.isotopeName;
                     ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(vandle.qdcPos);
                  }
                  {
                     //for the time difference between left and right
                     const std::string hname = "vandle_tdiff_"+isotope.isotopeName;
                     ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(vandle.tdiff);
                  }
                  if(vandle.vMulti>0){
                     //for the VANDLE multiplicity
                     const std::string hname = "vandle_vMulti_"+isotope.isotopeName;
                     ((TH1F*)GetOutputList()->FindObject(hname.c_str()))->Fill(vandle.vMulti);
                  }
               }
            }
         }
      }
   }//end loop through the merged_data TTree
   return kTRUE;
}

void AnamergerVANDLESelector::Terminate(){

   YamlReader yaml_reader("AnamergerVANDLESelector"); 
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

int AnamergerVANDLESelector::LoadCUTG(const std::string &icutname)
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

