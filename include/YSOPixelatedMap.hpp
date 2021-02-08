#ifndef YSOPIXELATEDMAP_HPP_
#define YSOPIXELATEDMAP_HPP_

#include "YSOMap.h"

class YSOPixelatedPositionData : public YSOPositionData {
public:
    YSOPixelatedPositionData(){};
    virtual ~YSOPixelatedPositionData(){};

    void SetIndex(Int_t id_x, Int_t id_y){
        fIndexX = id_x;
        fIndexY = id_y;
    }
    Bool_t PixelIsInside(Int_t ion_idx, Int_t ion_idy, const double &ion_r=-1) const;
    Int_t GetIndexX() const { return fIndexX; }
    Int_t GetIndexY() const { return fIndexY; }
    Double_t IonDistance(const Double_t &ion_x, const Double_t &ion_y) const;

protected:
    Int_t fIndexX;
    Int_t fIndexY;
};

class YSOPixelatedMap : public YSOMap {
public:
    YSOPixelatedMap(){}
    YSOPixelatedMap(std::string fname):YSOMap(){ LoadPositionParameters(fname); }
    virtual ~YSOPixelatedMap(){}

    void LoadPositionParameters(std::string fname) override;
    Bool_t IsInside(const Double_t &beta_x,const Double_t &beta_y,const Double_t &ion_x,const Double_t &ion_y,const Double_t &ion_r=-1) override;
    Int_t GenerateIonMap(const Int_t &num_div);

protected:
    std::map<Int_t, std::vector<YSOPositionData*>> fIonMap;
    Int_t fIonNumDiv = 0;
    Double_t fIonRangeX = 0;
    Double_t fIonRangeY = 0;
    Double_t fIonMinX = 0;
    Double_t fIonMinY = 0;
    Int_t GetIonId(const Double_t &x, const Double_t &y) const {
        return (Int_t)(((double)x-fIonMinX)/fIonRangeX*fIonNumDiv) + (fIonNumDiv)*((Int_t)(((double)y-fIonMinY)/fIonRangeY*fIonNumDiv));
    }
};
#endif