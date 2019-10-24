// AnamergerSparseSelector for PROOF created by Rin Yokoyama on 7/21/2017

#ifndef ANAMERGER_SPARSE_SELECTOR_H
#define ANAMERGER_SPARSE_SELECTOR_H

#include <TApplication.h>
#include <TChain.h>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "BigRIPSTreeData.h"
#include "CorrectedVANDLEData.h"
#include "OutputTreeData.hpp"
#include "ProcessorRootStruc.hpp"
#include "PspmtData.hpp"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"
#include "TMath.h"
#include "TParameter.h"
#include "TProofServ.h"
#include "TSelector.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

class AnamergerSparseSelector : public TSelector {
   public:
    AnamergerSparseSelector(TTree* = 0);
    virtual ~AnamergerSparseSelector();

    virtual Int_t Version() const { return 2; }
    virtual void Init(TTree* mergedData);
    virtual void Begin(TTree* mergedData);
    virtual void SlaveBegin(TTree* mergedData);
    virtual Bool_t Notify() { return kTRUE; }
    virtual Bool_t Process(Long64_t entry);
    virtual void SetOption(const char* option) { fOption = option; }
    virtual void SetObject(TObject* obj) { fObject = obj; }
    virtual void SetInputList(TList* input) { fInput = input; }
    virtual TList* GetOutputList() const { return fOutput; }
    virtual void SlaveTerminate() { tree_reader_.SetTree((TTree*)nullptr); }
    virtual void Terminate();
    void SetTimeWindow(const Double_t& time_window) { time_window_ = time_window; }

    void SetOutputFileName(const std::string& file_name) {
        output_file_name_ = file_name;
    }

   protected:
    TTreeReader tree_reader_;
    TTreeReaderValue<OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta_;
    TTreeReaderValue<std::vector<processor_struct::CLOVERS>> clover_vec_;
    TTreeReaderValue<std::vector<CorrectedVANDLEData>> vandle_vec_;
    ULong64_t total_entry_;

    // array for histograms
    TObjArray* fHistArray = nullptr;
    // output file
    TFile* fOutputFile = nullptr;
    std::string output_file_name_;
    TF1* n_correction = nullptr;
    Double_t time_window_;

    ClassDef(AnamergerSparseSelector, 1)
};

#endif
