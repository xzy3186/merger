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
#include "TF1.h"
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
#include "TParameter.h"
#include "CorrectedVANDLEData.h"

class AnamergerSelector : public TSelector {
public:

	AnamergerSelector(TTree* = 0);
	virtual ~AnamergerSelector();

	virtual Int_t   Version() const { return 2; }
	virtual void    Init(TTree* mergedData);
	virtual void    Begin(TTree* mergedData);
	virtual void    SlaveBegin(TTree* mergedData);
	virtual Bool_t  Notify() { return kTRUE; }
	virtual Bool_t  Process(Long64_t entry);
	virtual void    SetOption(const char* option) { fOption = option; }
	virtual void    SetObject(TObject* obj) { fObject = obj; }
	virtual void    SetInputList(TList* input) { fInput = input; }
	virtual TList* GetOutputList() const { return fOutput; }
	virtual void    SlaveTerminate() {}
	virtual void    Terminate();
	void SetTimeWindow(const Double_t& time_window) { time_window_ = time_window; }

	void SetOutputFileName(const std::string& file_name) {
		output_file_name_ = file_name;
	}

protected:

	TTreeReader tree_reader_;
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta_;
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec_;
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec_;
	ULong64_t total_entry_;

	// array for histograms
	TObjArray* fHistArray;
	// output file
	TFile* fOutputFile;
	std::string output_file_name_;
	TF1* n_correction;
	Double_t time_window_;

	ClassDef(AnamergerSelector, 1)
};

#endif
