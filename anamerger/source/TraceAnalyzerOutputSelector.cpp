#include "TraceAnalyzerOutputSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(TraceAnalyzerOutputSelector);

TraceAnalyzerOutputSelector::TraceAnalyzerOutputSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                          pspmt_(tree_reader_, "pspmt"),
                                                          output_file_name_("anamerger_output.root") {
}

TraceAnalyzerOutputSelector::~TraceAnalyzerOutputSelector()
{
	if (fHistArray) {
		delete fHistArray;
		fHistArray = nullptr;
	}
	if (fOutputFile) {
		delete fOutputFile;
		fOutputFile = nullptr;
	}
}

void TraceAnalyzerOutputSelector::Begin(TTree* mergedData) {
    GetOutputList()->Clear();
    if (fInput) {
        TNamed* named = (TNamed*)fInput->FindObject("output_file_name");
        if (named)
            output_file_name_ = named->GetTitle();
    }
}

void TraceAnalyzerOutputSelector::SlaveBegin(TTree* mergedData)
{
	if (fHistArray) {
		delete fHistArray;
		fHistArray = nullptr;
	}
	fHistArray = new TObjArray();

	fHistArray->Add(new TH2F("Tib_ClvE", "ClvE_Tib", 4000, 0, 4000, 1000, -3, 3));

	//adding histograms to output list
	TIter next(fHistArray);
	while (auto hist = next()) {
		GetOutputList()->Add(hist);
	}

	t_entry_ = 0;

	if (fInput) {
	//	TParameter<Double_t>* time_window = (TParameter<Double_t>*)fInput->FindObject("TimeWindow");
	//	if (time_window)
	//		time_window_ = time_window->GetVal();
	//	else
	//		time_window_ = 1.0;
	}

	if (gProofServ) {
		const TString msg = TString::Format("SlaveBegin() of Ord = %s called. %d histograms are initialized.",
			gProofServ->GetOrdinal(), GetOutputList()->GetEntries());
		gProofServ->SendAsynMessage(msg);
	}
	else {
		std::cout << "SalveBegin() called. " << GetOutputList()->GetEntries() << " histograms are initialized." << std::endl;
	}

	return;
}

void TraceAnalyzerOutputSelector::Init(TTree* pspmt) {
    tree_reader_.SetTree(pspmt);
    return;
}

Bool_t TraceAnalyzerOutputSelector::Process(Long64_t entry) {
	t_entry_++;
	tree_reader_.SetLocalEntry(entry);
	{
		auto pspmt = pspmt_.Get();
		if (!pspmt)
			return kTRUE;
		int anode_hits = 0;
		if (pspmt->high_gain_)
	}//end loop through the mergedData TTree

	return kTRUE;
}

void TraceAnalyzerOutputSelector::Terminate() {
    fOutputFile = new TFile(output_file_name_.c_str(), "recreate");
    std::cout << "[TraceAnalyzerOutputSelector::Terminate()]: output file: " << output_file_name_ << std::endl;
    // write the histograms
    TIter next(GetOutputList());
    while (TObject* obj = next()) {
        std::cout << "[TraceAnalyzerOutputSelector::Terminate]: writing " << obj->GetName() << " to file." << std::endl;
        obj->Write();
    }

	fOutputFile->Close();

	if (fOutputFile) {
		delete fOutputFile;
		fOutputFile = nullptr;
	}

    return;
}
