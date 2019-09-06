// CorrectionSelector for PROOF created by Rin Yokoyama on 8/29/2019

#ifndef CORRECTION_SELECTOR_H
#define CORRECTION_SELECTOR_H

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
#include "TProofOutputFile.h"
#include "TProofServ.h"
#include "TMath.h"
#include "PspmtData.hpp"
#include "BigRIPSTreeData.h"
#include "ProcessorRootStruc.hpp"
#include "OutputTreeData.hpp"
#include "TParameter.h"
#include "CorrectedVANDLEData.h"
#include "VANDLEToFCorrector.h"

class CorrectionSelector : public TSelector {
public:

	CorrectionSelector(TTree* = 0);
	virtual ~CorrectionSelector();

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
	virtual void    SlaveTerminate();
	virtual void    Terminate();

	void SetFileName(const std::string& name) { file_name_ = name; }
	void SetCorrectorConfigName(const std::string& name) { vandle_corrector_config_ = name; }

protected:

	TTreeReader tree_reader_;
	TTreeReaderValue <OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>>> beta_;
	TTreeReaderValue <std::vector<processor_struct::CLOVERS>> clover_vec_;
	TTreeReaderValue <std::vector<processor_struct::VANDLES>> vandle_vec_;
	std::vector<CorrectedVANDLEData> corrected_vandle_vec_;
	ULong64_t total_entry_;

	// output file
	TFile* fOutputFile = nullptr;
	TProofOutputFile* fProofFile = nullptr;
	TTree* fOutputTree = nullptr;
	OutputTreeData<PspmtData, OutputTreeData<PspmtData, TreeData>> beta_data_;
	TBranch* fBetaBranch;
	TBranch* fCloverBranch;
	TBranch* fVandleBranch;
	Long64_t n_events = 0;
	std::string file_name_;
	std::string proof_output_location_;
	std::string vandle_corrector_config_;
	Bool_t use_proof_;
	VANDLEToFCorrector* corrector_ = nullptr;
	
	void SetBranch();

	ClassDef(CorrectionSelector, 1)
};

#endif
