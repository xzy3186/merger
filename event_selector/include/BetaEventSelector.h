// BetaEventSelector for PROOF created by Rin Yokoyama on 2/7/2017

#ifndef BETA_EVENT_SELECTOR_H
#define BETA_EVENT_SELECTOR_H

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
#include "PaassRootStruct.hpp"
#include "OutputTreeData.hpp"
#include "TParameter.h"
#include "CorrectedVANDLEData.h"
#include "TBetaEventList.h"

class BetaEventSelector : public TSelector {
public:

	BetaEventSelector(TTree* = 0);
	virtual ~BetaEventSelector();

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
	virtual void    SlaveTerminate() { tree_reader_.SetTree((TTree*)nullptr); }
	virtual void    Terminate();
	void SetEventList(TBetaEventList* event_list) {
		fBetaEventList = event_list;
	}
	void SetTimeWindow(const Double_t& tw) {
		time_window_ = tw;
	}

protected:

	TTreeReader tree_reader_;
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta_;
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec_;
	TTreeReaderValue <std::vector<CorrectedVANDLEData>> vandle_vec_;
	TTreeReaderValue <std::vector<processor_struct::GAMMASCINT>> gamma_scint_vec_;
	ULong64_t total_entry_;

	TBetaEventList* fBetaEventList = nullptr;

	Double_t time_window_;

	ClassDef(BetaEventSelector, 1)
};

#endif
