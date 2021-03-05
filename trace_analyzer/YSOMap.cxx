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

Int_t YSOMap::GenerateMap(const Int_t &num_div){
  /* Divides beta position image into num_div*num_div grids and generates a map of vectors of YSOPositionData */
  /* in order to make searching a closest pixel faster. */
  fNumDiv = num_div;
  {
    auto lambda = [] (const YSOPositionData *a, const YSOPositionData *b){return a->GetBetaX() < b->GetBetaX();};
    const Double_t max = (*std::max_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetBetaX();
    const Double_t min = (*std::min_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetBetaX();
    fRangeX = max - min;
    fMinX = min;
    std::cout << "[YSOMap]: x_range: " << min << ", " << max << std::endl;
  }
  {
    auto lambda = [] (const YSOPositionData *a, const YSOPositionData *b){return a->GetBetaY() < b->GetBetaY();};
    const Double_t max = (*std::max_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetBetaY();
    const Double_t min = (*std::min_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetBetaY();
    fRangeY = max - min;
    fMinY = min;
    std::cout << "[YSOMap]: y_range: " << min << ", " << max << std::endl;
  }
  for(int i1=0; i1<num_div+1; ++i1){
    for(int i2=0; i2<num_div+1; ++i2){
      fMap.emplace(std::make_pair(i1+num_div*i2, std::vector<YSOPositionData*>()));
    }
  }
  for(const auto pos: fVectorOfYSOPositions){
    const Int_t key = GetId(pos->GetBetaX(),pos->GetBetaY());
    //std::cout << "[YSOMap]: x, y, key: " << pos.GetBetaX() << ", " << pos.GetBetaY() << ", " << key << std::endl;
    fMap[key].push_back(pos);
  }
  std::cout << "[YSOMap]: number of grids generated: " << fMap.size() << std::endl;
  for(auto const& pos_vec: fMap){
    std::cout << "[YSOMap]: grid #" << pos_vec.first << ", number of entries: " << pos_vec.second.size() << std::endl;
  }
  return 0;
}

Bool_t YSOMap::IsInside(const Double_t &beta_x,const Double_t &beta_y,
  const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r) {
  /* returns if the mapped position of (beta_x, beta_y) is in the range of ion_r from */
  /* the (ion_x, ion_y) position. */

  const YSOPositionData* p_closest = nullptr;
  Double_t distance_min = 1E+6;
  auto lambda = [&d_min = distance_min, &bx = beta_x, &by = beta_y] (const std::vector<YSOPositionData*> &pos_vec) {
    const YSOPositionData* p;
    for( auto pos : pos_vec ){
      const Double_t dist = pos->Distance(bx,by);
      if(dist<d_min) {
        d_min = dist;
        p = pos;
      }
    }
    return p;
  };
  if(fNumDiv){
    /* searches the closest pixel to the given beta position from the grid id-1, id, id+1 */
    for(int i=-1; i<2; ++i){
      if(i<0||i>fNumDiv)
        continue;
      for(int i2=-1; i2<2; ++i2){
        if(i2<0||i2>fNumDiv)
          continue;
        for( auto pos : fMap[GetId(beta_x+i,beta_y+i2)] ){
          if(pos->Distance(beta_x,beta_y)<distance_min){
            p_closest = pos;
            distance_min = pos->Distance(beta_x,beta_y);
          }
        }
      }
    }
  }
  else{ /*searches the closest pixel to the given beta position from the entire map */
    for( auto pos : fVectorOfYSOPositions ){
      if(pos->Distance(beta_x,beta_y)<distance_min) {
        p_closest = pos;
        distance_min = pos->Distance(beta_x,beta_y);
      }
    }
  }
  if(!p_closest)
    return false;
  if(p_closest->IonIsInside(ion_x,ion_y,ion_r))
    return true;
  else
    return false;
}

void YSOPositionData::SetPositions(const Double_t &beta_x,const Double_t &beta_y,const Double_t &ion_x,const Double_t &ion_y){
  fBetaX = beta_x;
  fBetaY = beta_y;
  fIonX = ion_x;
  fIonY = ion_y;
  //std::cout << "BetaX, BetaY, IonX, IonY: " << fBetaX << " " << fBetaY << " " << fIonX << " " << fIonY << std::endl;
  return;
}

Bool_t YSOPositionData::BetaIsInside(const Double_t &beta_x,const Double_t &beta_y) const {
  return (TMath::Power((Double_t)beta_x-(Double_t)fBetaX,2.)+TMath::Power((Double_t)beta_y-(Double_t)fBetaY,2.)<kBetaR*kBetaR);
}

Double_t YSOPositionData::Distance(const Double_t &beta_x,const Double_t &beta_y) const {
  return (TMath::Power((Double_t)beta_x-(Double_t)fBetaX,2.)+TMath::Power((Double_t)beta_y-(Double_t)fBetaY,2.));
}

Bool_t YSOPositionData::IonIsInside(const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r) const {
  if(ion_r<0)
    return (TMath::Power((Double_t)ion_x-(Double_t)fIonX,2.)+TMath::Power((Double_t)ion_y-(Double_t)fIonY,2.)<kIonR*kIonR);
  else
    return (TMath::Power((Double_t)ion_x-(Double_t)fIonX,2.)+TMath::Power((Double_t)ion_y-(Double_t)fIonY,2.)<ion_r*ion_r);
}
