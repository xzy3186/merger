// // // // // // // // // // // // // // // // // // 
// 
// Definition of an structure which gathers
// all the histograms associated with ONE isotope
// 
// VERSION 2.0. NEW HISTOGRAMS ADDED!!
// 
// // // // // // // // // // // // // // // // // // 


#ifndef __h_isotopeFastID_hpp__
#define __h_isotopeFastID_hpp__

#include "TFile.h"
#include "TCutG.h"
#include "TList.h"
#include "TObject.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TKey.h"
#include "TIterator.h"
#include "TMath.h"

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

class hIsotope
{
  public:
    hIsotope(){}
    hIsotope( TCutG * aCut, TList* outputList );
    hIsotope( std::string isoname, Double_t a, Double_t b, Double_t x0, Double_t y0, TList* outputList );
    ~hIsotope();
    void initializeCut(TCutG* aCut);
    void initializeHistos(TList* outputList);
    bool IsInside(Double_t x, Double_t y) const;

    /// double numberOfImplants
    int32_t numberOfImplants;
    /// double numberOfImplants
    int32_t numberOfPID;
    /// number of correlated implants in an event
    int32_t numberOfCorrelatedImp;
    /// myCutG: cut for PID
    TCutG * myCutG;
    Double_t ellipseA;
    Double_t ellipseB;
    Double_t ellipseX0;
    Double_t ellipseY0;

    TObjArray *hist_array_;
    std::string isotopeName;
};

#endif //

