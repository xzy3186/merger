
#ifndef YSOMAP_H
#define YSOMAP_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <TMath.h>

class YSOPositionData{
  /* an object to map a pixel position in the beta image to the ion image*/
public:
  YSOPositionData(){};
  virtual ~YSOPositionData(){};

  void SetPositions(const Double_t &beta_x,const Double_t &beta_y,const Double_t &ion_x,const Double_t &ion_y);
  Bool_t BetaIsInside(const Double_t &beta_x,const Double_t &beta_y) const;
  Bool_t IonIsInside(const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r=-1) const;
  Double_t Distance(const Double_t &beta_x,const Double_t &beta_y) const;
  Double_t GetBetaX() const {return fBetaX;}
  Double_t GetBetaY() const {return fBetaY;}
  Double_t GetIonX() const {return fIonX;}
  Double_t GetIonY() const {return fIonY;}

protected:
  Double_t fBetaX;
  Double_t fBetaY;
  Double_t fIonX;
  Double_t fIonY;
  Double_t kBetaR = 0.007;
  Double_t kIonR = 0.04;
};

class YSOMap {
public:
  YSOMap(){};
  YSOMap(std::string fname);
  virtual ~YSOMap(){
    for(auto data: fVectorOfYSOPositions){
      if(data)
        delete data;
    }
  };

  virtual void LoadPositionParameters(std::string fname);
  virtual Bool_t IsInside(const Double_t &beta_x,const Double_t &beta_y,const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r=-1);
  Int_t GenerateMap(const Int_t &num_div=10);
protected:
  std::vector<YSOPositionData*> fVectorOfYSOPositions;
  std::map<Int_t, std::vector<YSOPositionData*>> fMap;
  Int_t fNumDiv = 0;
  Double_t fRangeX = 0;
  Double_t fRangeY = 0;
  Double_t fMinX = 0;
  Double_t fMinY = 0;
  Int_t GetId(const Double_t &x, const Double_t &y) const {
    const auto idx = (Int_t)(((double)x-fMinX)/fRangeX*fNumDiv);
    const auto idy = (Int_t)(((double)y-fMinY)/fRangeY*fNumDiv);
    return idx + fNumDiv * idy;
  }
};

#endif