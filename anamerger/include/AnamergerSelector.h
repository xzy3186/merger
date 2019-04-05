// AnamergerSelector for PROOF created by Rin Yokoyama on 7/21/2017

#ifndef ANAMERGER_SELECTOR_H
#define ANAMERGER_SELECTOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TSelector.h"
#include "TProofServ.h"
#include "TMath.h"
#include "PspmtData.hpp"
#include "BigRIPSTreeData.h"
#include "ProcessorRootStruc.hpp"
#include "OutputTreeData.hpp"

class AnamergerSelector : public TSelector {
public :

  AnamergerSelector(TTree * =0);
  virtual ~AnamergerSelector();

  virtual Int_t   Version() const { return 2; }
  virtual void    Init(TTree* mergedData);
  virtual void    Begin(TTree *mergedData);
  virtual void    SlaveBegin(TTree *mergedData);
  virtual Bool_t  Notify() { return kTRUE;}
  virtual Bool_t  Process(Long64_t entry);
  virtual void    SetOption(const char *option) { fOption = option; }
  virtual void    SetObject(TObject *obj) { fObject = obj; }
  virtual void    SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate(){}
  virtual void    Terminate();

protected:

  TTreeReader tree_reader_;
  TTreeReaderValue <OutputTreeData<PspmtData,OutputTreeData<PspmtData,TreeData>>> beta_;
  TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec_;
  TTreeReaderValue <std::vector<processor_struct::VANDLES>> vandle_vec_;
  ULong64_t total_entry_;

  // array for histograms
  TObjArray* fHistArray;
  // output file
  TFile * fOutputFile;

  ClassDef(AnamergerSelector,1)
};

#endif
