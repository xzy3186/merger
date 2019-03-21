#ifndef YSOMAP_H
#define YSOMAP_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <TMath.h>

class YSOPositionData{
public:
  YSOPositionData(){};
  virtual ~YSOPositionData(){};

  void SetPositions(Double_t beta_x,Double_t beta_y,Double_t ion_x,Double_t ion_y);
  Bool_t BetaIsInside(Double_t beta_x,Double_t beta_y) const;
  Bool_t IonIsInside(Double_t ion_x,Double_t ion_y,Double_t ion_r=-1) const;
  Double_t Distance(Double_t beta_x,Double_t beta_y) const;

protected:
  Double_t fBetaX;
  Double_t fBetaY;
  Double_t fIonX;
  Double_t fIonY;
  const Double_t kBetaR = 0.007;
  const Double_t kIonR = 0.04;
};

class YSOMap {
public:
  YSOMap(){};
  YSOMap(std::string fname);
  virtual ~YSOMap(){};

  void LoadPositionParameters(std::string fname);
  Bool_t IsInside(Double_t beta_x,Double_t beta_y,Double_t ion_x,Double_t ion_y,Double_t ion_r=-1) const;
protected:
  std::vector<YSOPositionData*> fVectorOfYSOPositions;
};

#endif
