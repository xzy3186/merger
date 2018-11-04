// #################################################################################################################
// 
// # Alvaro Tolosa Delgado @ IFIC(Valencia,Spain)  alvaro.tolosa@ific.uv.es
// # Copyright (c) 2016 Jorge Agramunt & Alvaro Tolosa. All rights reserved.
// 
// #################################################################################################################

#include "AnamergerSelector.h"
#include <iostream>
#include <fstream>
#include <string>

ClassImp(AnamergerSelector);
  
AnamergerSelector::AnamergerSelector(TTree* mergedData):
   aReader (mergedData),
   bigrips  (aReader, "bigrips."),
   implant  (aReader, "implantation."),
   beta     (aReader, "beta."),
   neutron  (aReader, "neutron."),
   gamma    (aReader, "gamma."),
   ancillary(aReader, "ancillary.") 
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
   
  loadCUTG(ANAMERGER_PATH + "referenceCuts.txt");

  fYSOMap = new YSOMap(ANAMERGER_PATH + "yso_map.txt");

  fHistArray = new TObjArray();

  fHistArray->Add(new TH2F("PID","PID",500,2.6,2.9,500,24,36));
  fHistArray->Add(new TH1F("countPID","countPID",500,0,500));
  fHistArray->Add(new TH2F("beta","beta",500,0,1,500,0,1));

  //adding histograms to output list
  TIter next(fHistArray);
  while( TH2D* hist = (TH2D*)next() ){
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
  aReader.SetTree( mergedData );
  return;
}


Bool_t AnamergerSelector::Process(Long64_t entry){

  aReader.SetLocalEntry(entry);
  {
  }//end loop through the mergedData TTree

  return kTRUE;
}

void AnamergerSelector::Terminate(){
 
  {
    const string fname = ANAMERGER_PATH + "anamerger_output.root";
    fOutputFile = new TFile(fname.c_str(),"recreate");
  }

  // write the parameters
  {
    std::string parString("Beta-neutron time correlation cuts. Long_t DTbnlow= ");
    parString+=std::to_string(DTbnlow);
    parString+=" ns. Long_t DTbnhigh= ";
    parString+=std::to_string(DTbnhigh);
    parString+=" ns.";
    TNamed parTNamed("par1", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Beta-gamma time correlation cuts. Long_t DTgblow= ");
    parString+=std::to_string(DTgblow);
    parString+=" ns. Long_t DTgbhigh= ";
    parString+=std::to_string(DTgbhigh);
    parString+=" ns.";
    TNamed parTNamed("par2", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Beta-implant time gate for gammas. Long_t DTibhigh= ");
    parString+=std::to_string(DTibhigh);
    parString+=" ns.";
    TNamed parTNamed("par3", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Beta-F11 time cut window. Long_t DTfblow= ");
    parString+=std::to_string(DTfblow);
    parString+=" ns. Long_t DTfbhigh= ";
    parString+=std::to_string(DTfbhigh);
    parString+=" ns.";
    TNamed parTNamed("par4", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Neutron-F11 time cut window. Long_t DTfnhigh= ");
    parString+=std::to_string(DTfnhigh);
    parString+=" ns.";
    TNamed parTNamed("par5", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Neutron energy cuts. Enlow= ");
    parString+=std::to_string(Enlow);
    parString+=". keV Enhigh= ";
    parString+=std::to_string(Enhigh);
    parString+=". keV. Eplow= ";
    parString+=std::to_string(Eplow);
    parString+=". keV.";
    TNamed parTNamed("par6", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Beta-implant max. X,Y distance. Double_t DSib= ");
    parString+=std::to_string(DSib);
    parString+=" pixels (square 2*DSib x 2*DSib).";
    TNamed parTNamed("par7", parString.c_str() );
    parTNamed.Write(0,2,0);
  }
  {
    std::string parString("Beta energy cut. Double_t Eblow= ");
    parString+=std::to_string(Eblow);
    parString+=" keV.";
    TNamed parTNamed("par7", parString.c_str() );
    parTNamed.Write(0,2,0);
  }

  // write the histograms
  TIter next(GetOutputList());
  while( TObject* obj = next() ){
    obj->Write();
  }

  fOutputFile->Close();
  fHistArray->Clear();
  
  //delete fHistArray;
  //delete fYSOMap;
  //delete fOutputFile;

  return ;
}


int AnamergerSelector::loadCUTG(std::string icutname)
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
      Double_t ellipse_a, ellipse_b, ellipse_x0, ellipse_y0;
      fcut >> ellipse_a;
      fcut >> ellipse_b;
      fcut >> ellipse_x0;
      fcut >> ellipse_y0;
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
 
