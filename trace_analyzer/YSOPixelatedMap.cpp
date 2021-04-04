#include "YSOPixelatedMap.hpp"

void YSOPixelatedMap::LoadPositionParameters(std::string fname){
  fVectorOfYSOPositions.clear();
  std::ifstream ifile(fname);
  if(!ifile){
    std::cout << "[YSOMap]: cannot open file " << fname << std::endl;
    return;
  }
  while( !ifile.eof() ){
    Double_t bx, by, ix, iy, distance;
    Int_t id_x, id_y;
    ifile >> id_x;
    ifile >> id_y;
    ifile >> bx;
    ifile >> by;
    ifile >> ix;
    ifile >> iy;
    ifile >> distance;
    YSOPixelatedPositionData *pos = new YSOPixelatedPositionData();
    pos->SetPositions(bx,by,ix,iy);
    pos->SetIndex(id_x, id_y);
    fVectorOfYSOPositions.push_back(pos);
  }
  std::cout << "[YSOPixelatedMap]: loaded " << fVectorOfYSOPositions.size() << " pixiels from " << fname << std::endl;
}

Int_t YSOPixelatedMap::GenerateIonMap(const Int_t &num_div){
  /* Divides beta position image into num_div*num_div grids and generates a map of vectors of YSOPositionData */
  /* in order to make searching a closest pixel faster. */
  fIonNumDiv = num_div;
  {
    auto lambda = [] (const YSOPositionData *a, const YSOPositionData *b){return a->GetIonX() < b->GetIonX();};
    const Double_t max = (*std::max_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetIonX();
    const Double_t min = (*std::min_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetIonX();
    fIonRangeX = max - min;
    fIonMinX = min;
    std::cout << "[YSOPixelatedMap]: x_range: " << min << ", " << max << std::endl;
  }
  {
    auto lambda = [] (const YSOPositionData *a, const YSOPositionData *b){return a->GetIonY() < b->GetIonY();};
    const Double_t max = (*std::max_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetIonY();
    const Double_t min = (*std::min_element(fVectorOfYSOPositions.begin(),fVectorOfYSOPositions.end(), lambda))->GetIonY();
    fIonRangeY = max - min;
    fIonMinY = min;
    std::cout << "[YSOPixelatedMap]: y_range: " << min << ", " << max << std::endl;
  }
  for(int i1=0; i1<num_div+1; ++i1){
    for(int i2=0; i2<num_div+1; ++i2){
      fIonMap.emplace(std::make_pair(i1+num_div*i2, std::vector<YSOPositionData*>()));
    }
  }
  for(const auto pos: fVectorOfYSOPositions){
    const Int_t key = GetIonId(pos->GetIonX(),pos->GetIonY());
    //std::cout << "[YSOMap]: x, y, key: " << pos.GetBetaX() << ", " << pos.GetBetaY() << ", " << key << std::endl;
    if (key >= 0)
      fIonMap[key].push_back(pos);
  }
  std::cout << "[YSOPixelatedMap]: number of grids generated: " << fIonMap.size() << std::endl;
  for(auto const& pos_vec: fIonMap){
    std::cout << "[YSOPixelatedMap]: grid #" << pos_vec.first << ", number of entries: " << pos_vec.second.size() << std::endl;
  }
  return 0;
}

Bool_t YSOPixelatedMap::IsInside(const Double_t &beta_x,const Double_t &beta_y,
  const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r) {
  /* returns if the index of (beta_x, beta_y) pixel is in the range of ion_r from */
  /* the (ion_x, ion_y) index. */

  const YSOPixelatedPositionData* p_closest = nullptr;
  Double_t distance_min = 1E+20;
  auto lambda = [&d_min = distance_min, &bx = beta_x, &by = beta_y] (const std::vector<YSOPositionData*> &pos_vec) {
    const YSOPixelatedPositionData* p;
    for( auto pos : pos_vec ){
      const Double_t dist = pos->Distance(bx,by);
      if(dist<d_min) {
        d_min = dist;
        p = (YSOPixelatedPositionData*)pos;
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
            p_closest = (YSOPixelatedPositionData*)pos;
            distance_min = pos->Distance(beta_x,beta_y);
          }
        }
      }
    }
  }
  else{ /*searches the closest pixel to the given beta position from the entire map */
    for( auto pos : fVectorOfYSOPositions ){
      if(pos->Distance(beta_x,beta_y)<distance_min) {
        p_closest = (YSOPixelatedPositionData*)pos;
        distance_min = pos->Distance(beta_x,beta_y);
      }
    }
  }
  if(!p_closest)
    return false;

  const YSOPixelatedPositionData* p_ion_closest = nullptr;
  distance_min = 1E+20;
  if(fIonNumDiv){
    /* searches the closest pixel to the given ion position from the grid id-1, id, id+1 */
    for(int i=-1; i<2; ++i){
      if(i<0||i>fIonNumDiv)
        continue;
      for(int i2=-1; i2<2; ++i2){
        if(i2<0||i2>fIonNumDiv)
          continue;
        for( auto pos : fIonMap[GetIonId(ion_x+i,ion_y+i2)] ){
          auto pix_pos = (YSOPixelatedPositionData*)pos;
          if(pix_pos->IonDistance(ion_x,ion_y)<distance_min){
            p_ion_closest = pix_pos;
            distance_min = pix_pos->IonDistance(ion_x,ion_y);
          }
        }
      }
    }
  }
  else{ /*searches the closest pixel to the given beta position from the entire map */
    for( auto pos : fVectorOfYSOPositions ){
      auto pix_pos = (YSOPixelatedPositionData*)pos;
      if(pix_pos->IonDistance(ion_x,ion_y)<distance_min) {
        p_ion_closest = pix_pos;
        distance_min = pix_pos->IonDistance(ion_x,ion_y);
      }
    }
  }
      //std::cout << "Ion (idx,idy,x,y) = (" << p_ion_closest->GetIndexX() << ", " << p_ion_closest->GetIndexY() << ", " << p_ion_closest->GetIonX() << ", " << p_ion_closest->GetIonY() << "), ";
      //std::cout << "Bet (idx,idy,x,y) = (" << p_closest->GetIndexX() << ", " << p_closest->GetIndexY() << ", " << p_closest->GetBetaX() << ", " << p_closest->GetBetaY() << ")";
  if(!p_ion_closest)
    return false;
  if(p_closest->PixelIsInside(p_ion_closest->GetIndexX(), p_ion_closest->GetIndexY(), ion_r)){
    //std::cout << "true" << std::endl;
    return true;
  }
  else{
    //std::cout << "false" << std::endl;
    return false;
  }
}

Bool_t YSOPixelatedPositionData::PixelIsInside(Int_t ion_idx, Int_t ion_idy, const Double_t &ion_r) const {
  if(ion_r<0)
    return (TMath::Power((Double_t)fIndexX-(Double_t)ion_idx,2.)+TMath::Power((Double_t)fIndexY-(Double_t)ion_idy,2.)<kIonR*kIonR);
  else
    return (TMath::Power((Double_t)fIndexX-(Double_t)ion_idx,2.)+TMath::Power((Double_t)fIndexY-(Double_t)ion_idy,2.)<ion_r*ion_r);
}

Double_t YSOPixelatedPositionData::IonDistance(const Double_t &ion_x,const Double_t &ion_y) const {
  return (TMath::Power((Double_t)ion_x-(Double_t)fIonX,2.)+TMath::Power((Double_t)ion_y-(Double_t)fIonY,2.));
}

const YSOPixelatedPositionData* YSOPixelatedMap::FindClosestPixelBeta(const Double_t &beta_x, const Double_t &beta_y) {
  const YSOPixelatedPositionData* p_closest = nullptr;
  Double_t distance_min = 1E+20;
  auto lambda = [&d_min = distance_min, &bx = beta_x, &by = beta_y] (const std::vector<YSOPositionData*> &pos_vec) {
    const YSOPixelatedPositionData* p;
    for( auto pos : pos_vec ){
      const Double_t dist = pos->Distance(bx,by);
      if(dist<d_min) {
        d_min = dist;
        p = (YSOPixelatedPositionData*)pos;
      }
    }
    return p;
  };
  if(fNumDiv){
    /* searches the closest pixel to the given beta position from the grid id-1, id, id+1 */
    for(int i=-1; i<2; ++i){
      for(int i2=-1; i2<2; ++i2){
        const Int_t pixel_id = GetId(beta_x+i*fRangeX/fNumDiv, beta_y+i2*fRangeY/fNumDiv);
        if (pixel_id < 0||pixel_id > fNumDiv*fNumDiv)
          continue;
        for( auto pos : fMap[pixel_id] ){
          if(pos->Distance(beta_x,beta_y)<distance_min){
            p_closest = (YSOPixelatedPositionData*)pos;
            distance_min = pos->Distance(beta_x,beta_y);
          }
        }
      }
    }
  }
  else{ /*searches the closest pixel to the given beta position from the entire map */
    for( auto pos : fVectorOfYSOPositions ){
      if(pos->Distance(beta_x,beta_y)<distance_min) {
        p_closest = (YSOPixelatedPositionData*)pos;
        distance_min = pos->Distance(beta_x,beta_y);
      }
    }
  }
  return p_closest;
}

const YSOPixelatedPositionData* YSOPixelatedMap::FindClosestPixelIon(const Double_t &ion_x, const Double_t &ion_y) {
  const YSOPixelatedPositionData* p_ion_closest = nullptr;
  Double_t distance_min = 1E+20;
  if(fIonNumDiv){
    /* searches the closest pixel to the given ion position from the grid id-1, id, id+1 */
    for(int i=-1; i<2; ++i){
      //if(i<0||i>fIonNumDiv)
      //  continue;
      for(int i2=-1; i2<2; ++i2){
        //if(i2<0||i2>fIonNumDiv)
        //  continue;
        const Int_t pixel_id = GetIonId(ion_x+i*fIonRangeX/fIonNumDiv, ion_y+i2*fIonRangeY/fIonNumDiv);
        if (pixel_id < 0||pixel_id > fIonNumDiv*fIonNumDiv)
          continue;
        for( auto pos : fIonMap[pixel_id] ){
          auto pix_pos = (YSOPixelatedPositionData*)pos;
          if(pix_pos->IonDistance(ion_x,ion_y)<distance_min){
            p_ion_closest = pix_pos;
            distance_min = pix_pos->IonDistance(ion_x,ion_y);
          }
        }
      }
    }
  }
  else{ /*searches the closest pixel to the given beta position from the entire map */
    for( auto pos : fVectorOfYSOPositions ){
      auto pix_pos = (YSOPixelatedPositionData*)pos;
      if(pix_pos->IonDistance(ion_x,ion_y)<distance_min) {
        p_ion_closest = pix_pos;
        distance_min = pix_pos->IonDistance(ion_x,ion_y);
      }
    }
  }
  return p_ion_closest;
}