// AnamergerSelector for PROOF created by Rin Yokoyama on 7/21/2017

#ifndef ANAMERGER_VANDLE_SELECTOR_H
#define ANAMERGER_VANDLE_SELECTOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TStopwatch.h"
#include "TSelector.h"
#include "TProofServ.h"
#include "Rtypes.h"
#include "TMath.h"
#include "YamlReader.hpp"
#include "hIsotope.h"
#include "BigRIPSTreeData.h"
#include "ProcessorRootStruc.hpp"
#include "OutputTreeData.hpp"

class AnamergerVANDLESelector : public TSelector {
   public :

      AnamergerVANDLESelector(TTree * =0);
      virtual ~AnamergerVANDLESelector();

      virtual Int_t   Version() const { return 2; }
      virtual void    Init(TTree* merged_data);
      virtual void    Begin(TTree *merged_data);
      virtual void    SlaveBegin(TTree *merged_data);
      virtual Bool_t  Notify() { return kTRUE;}
      virtual Bool_t  Process(Long64_t entry);
      virtual void    SetOption(const char *option) { fOption = option; }
      virtual void    SetObject(TObject *obj) { fObject = obj; }
      virtual void    SetInputList(TList *input) { fInput = input; }
      virtual TList  *GetOutputList() const { return fOutput; }
      virtual void    SlaveTerminate(){}
      virtual void    Terminate();

      int LoadCUTG(const std::string &icutname);

   protected:
      std::vector <hIsotope> vectorIsotopes;
      const Double_t tini = 0;
      const Double_t tfin = 1e15;

      TTreeReader tree_reader_;
      //TTreeReaderValue <OutputTreeData<PixTreeEvent,TreeData>>    merged_event_;
      TTreeReaderValue <OutputTreeData<OutputTreeData<PixTreeEvent,TreeData>, PixTreeEvent>>    merged_event_;

      // array for histograms
      TObjArray* hist_array_;
      // output file
      TFile * output_file_;
      //YSOMap *yso_map_;

      ClassDef(AnamergerVANDLESelector,1)
};

#endif
