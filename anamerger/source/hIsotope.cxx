// // // // // // // // // // // // // // // // // // 
// 
// Definition of an structure which gathers
// all the histograms associated with ONE isotope
// 
// VERSION 2.0. NEW HISTOGRAMS ADDED!!
// 
// // // // // // // // // // // // // // // // // // 

#include "hIsotope.h"

hIsotope::hIsotope(TCutG * aCut, TList* outputList):
numberOfImplants(0),
numberOfPID(0),
isotopeName("")
{
  myCutG = nullptr;
  isotopeName = "";
  ellipseA = 0;
  ellipseB = 0;
  ellipseX0 = 0;
  ellipseY0 = 0;

  initializeCut( aCut );
  initializeHistos( outputList );
}

hIsotope::hIsotope( std::string isoname, Double_t a, Double_t b, Double_t x0, Double_t y0, TList* outputList):
numberOfImplants(0),
numberOfPID(0),
isotopeName("")
{
  myCutG = nullptr;
  isotopeName = isoname;
  ellipseA = a;
  ellipseB = b;
  ellipseX0 = x0;
  ellipseY0 = y0;

  initializeHistos( outputList );
}


hIsotope::~hIsotope(){
}

void hIsotope::initializeCut(TCutG * aCut)
{
  std::string tempName4Cut( aCut->GetName() );   
  tempName4Cut="i"+tempName4Cut;
  myCutG=(TCutG *)(aCut->Clone( tempName4Cut.c_str() ));
  return;
}

void hIsotope::initializeHistos(TList* outputList)
{
    if( myCutG ){
      // get the name of the isotope
      // it must be at the end
      std::string tempName4Cut( myCutG->GetName() );
      size_t lastindex = tempName4Cut.find_last_of("CUTG");
      if( lastindex == size_t(-1) ) lastindex = tempName4Cut.find_last_of("cutg");
      // the name of TCutG could be "blablaCUTG82ga"
      // and the name of the isotope will be "82ga"
      std::cout << lastindex << " " << (tempName4Cut.substr(3+1, tempName4Cut.size())).c_str() << std::endl;
      isotopeName = tempName4Cut.substr(4+1, tempName4Cut.size());
    }

    hist_array_ = new TObjArray();
    // histogram definition
    {
      std::string histName = "tof_"+isotopeName;
      TH1F* h = new TH1F(histName.c_str(),"tof vandle",10000,-1000,1000);
      h->SetXTitle("tof");
      h->SetYTitle("counts");
      hist_array_->Add(h); //ET histograms
    }

    TIter next(hist_array_);
    while( TObject* obj = next() ){ 
      outputList->Add(obj);
    }
   
  return;
}

bool hIsotope::IsInside(Double_t x, Double_t y) const
{
  if(myCutG){
    return myCutG->IsInside(x,y);
  }else{
    return (ellipseA*pow(x-ellipseX0,2)+ellipseB*pow(y-ellipseY0,2)<1);
  }
}
