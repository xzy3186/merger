// BaseSelector for PROOF created by Rin Yokoyama on 4/25/2019

#ifndef BASE_SELECTOR_H
#define BASE_SELECTOR_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TSelector.h"
#include "TProofServ.h"

template <class TIN, class TOUT>
class BaseSelector : public TSelector {
public :

  BaseSelector(TTree * =0);
  virtual ~BaseSelector();

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

  void SetOutputFileName(const std::string &file_name){
    output_file_name_ = file_name;
  }

protected:

  TTreeReader tree_reader_;
  TTreeReaderValue<T> reader_value_;
  ULong64_t total_entry_;

  // array for histograms
  TObjArray* fHistArray;
  // output file
  TFile * fOutputFile;
  std::string output_file_name_;

  ClassDef(BaseSelector,1)
};

#endif
