#include "YSOMap.h"

YSOMap::YSOMap(std::string fname){
  LoadPositionParameters(fname);
}

void YSOMap::LoadPositionParameters(std::string fname){
  fVectorOfYSOPositions.clear();
  std::ifstream ifile(fname);
  if(!ifile){
    std::cout << "[YSOMap]: cannot open file " << fname << std::endl;
    return;
  }
  while( !ifile.eof() ){
    Double_t bx, by, ix, iy;
    ifile >> bx;
    ifile >> by;
    ifile >> ix;
    ifile >> iy;
    YSOPositionData *pos = new YSOPositionData();
    pos->SetPositions(bx,by,ix,iy);
    fVectorOfYSOPositions.push_back(pos);
  }
  std::cout << "[YSOMap]: loaded " << fVectorOfYSOPositions.size() << " pixiels from " << fname << std::endl;
}

Bool_t YSOMap::IsInside(Double_t beta_x,Double_t beta_y,Double_t ion_x,Double_t ion_y,Double_t ion_r) const {

  YSOPositionData* p_closest = nullptr;
  Double_t distance_min = 1E+6;
  for( auto pos : fVectorOfYSOPositions ){
    if(pos->Distance(beta_x,beta_y)<distance_min) {
      p_closest = pos;
      distance_min = pos->Distance(beta_x,beta_y);
    }
  }
  if(!p_closest)
    return false;
  if(p_closest->IonIsInside(ion_x,ion_y,ion_r))
    return true;
  else
    return false;
}

void YSOPositionData::SetPositions(Double_t beta_x,Double_t beta_y,Double_t ion_x,Double_t ion_y){
  fBetaX = beta_x;
  fBetaY = beta_y;
  fIonX = ion_x;
  fIonY = ion_y;
  std::cout << "BetaX, BetaY, IonX, IonY: " << fBetaX << " " << fBetaY << " " << fIonX << " " << fIonY << std::endl;
  return;
}

Bool_t YSOPositionData::BetaIsInside(Double_t beta_x,Double_t beta_y) const {
  return (TMath::Power((Double_t)beta_x-(Double_t)fBetaX,2.)+TMath::Power((Double_t)beta_y-(Double_t)fBetaY,2.)<kBetaR*kBetaR);
}

Double_t YSOPositionData::Distance(Double_t beta_x,Double_t beta_y) const {
  return (TMath::Power((Double_t)beta_x-(Double_t)fBetaX,2.)+TMath::Power((Double_t)beta_y-(Double_t)fBetaY,2.));
}

Bool_t YSOPositionData::IonIsInside(Double_t ion_x,Double_t ion_y,Double_t ion_r) const {
  if(ion_r<0)
    return (TMath::Power((Double_t)ion_x-(Double_t)fIonX,2.)+TMath::Power((Double_t)ion_y-(Double_t)fIonY,2.)<kIonR*kIonR);
  else
    return (TMath::Power((Double_t)ion_x-(Double_t)fIonX,2.)+TMath::Power((Double_t)ion_y-(Double_t)fIonY,2.)<ion_r*ion_r);
}

