#include "CorrectionSelector.h"
#include <fstream>
#include <iostream>
#include <string>

ClassImp(CorrectionSelector);

CorrectionSelector::CorrectionSelector(TTree* mergedData) : tree_reader_(mergedData),
                                                            beta_(tree_reader_, "mergedBeta"),
                                                            clover_vec_(tree_reader_, "clover_vec_"),
                                                            gammascint_vec_(tree_reader_, "gamma_scint_vec_"),
                                                            vandle_vec_(tree_reader_, "vandle_vec_") {
}

CorrectionSelector::~CorrectionSelector() {
}

void CorrectionSelector::Begin(TTree* mergedData) {
    std::cout << "[CorrectionSelector]: Begin() called." << std::endl;
    GetOutputList()->Clear();
    if (fInput) {
        auto param = (TParameter<Bool_t>*)fInput->FindObject("use_proof");
        if (param)
            use_proof_ = true;
        else
            use_proof_ = false;
    } else {
        use_proof_ = false;
    }

    if (use_proof_) {
        // If this is a proof process, do nothing in Begin()
        std::cout << "[CorrectionSelector]: a proofserv found." << std::endl;
        return;
    } else {
        // If this is not a proof process, open output file and initialize tree
        std::cout << "[CorrectionSelector]: no proofserv found." << std::endl;
        std::cout << "[CorrectionSelector]: running with a single thread." << std::endl;
        file_name_ = file_name_ + ".root";
        if (fOutputFile) {
            delete fOutputFile;
            fOutputFile = nullptr;
        }
        if (fOutputTree) {
            delete fOutputTree;
            fOutputTree = nullptr;
        }
        //fOutputFile = new TFile(file_name_.c_str(), "RECREATE", "", ROOT::CompressionSettings(ROOT::kLZMA, 8));
        fOutputFile = new TFile(file_name_.c_str(), "RECREATE", "");
        fOutputTree = new TTree("mergedCorrectedBeta", "mergedCorrectedBeta");
        fOutputTree->Branch("corrected_vandle_vec", "std::vector<CorrectedVANDLEData>", &corrected_vandle_vec_);
        fOutputTree->SetDirectory(fOutputFile);

        if (corrector_) {
            delete corrector_;
            corrector_ = nullptr;
        }
        // create an instance of VANDLEToFCorrector
        std::cout << "[CorrectionSelector::Begin()]: creating VANDLEToFCorrector(" << vandle_corrector_config_ << ")" << std::endl;
        corrector_ = new VANDLEToFCorrector(vandle_corrector_config_);
    }
}

void CorrectionSelector::SlaveBegin(TTree* mergedData) {
    std::string filename;
    if (fInput) {
        {
            // read output file name.
            TNamed* named = (TNamed*)fInput->FindObject("output_file_prefix");
            if (named)
                filename = named->GetTitle();
            else
                filename = "corrected_tree";
        }
        {
            // read output file path
            TNamed* named = (TNamed*)fInput->FindObject("proof_output_location");
            if (named)
                proof_output_location_ = named->GetTitle();
            else
                proof_output_location_ = "./";
        }
        {
            // read corrector config file name
            TNamed* named = (TNamed*)fInput->FindObject("vandle_corrector_config");
            if (named)
                vandle_corrector_config_ = named->GetTitle();
            else
                vandle_corrector_config_ = "config_vandle_corrector.yaml";
        }
        {
            // read output branch names
            auto list = (TList*)fInput->FindObject("output_branches");
            if (list) {
                output_branches_.clear();
                TIter next(list);
                while (auto obj = next()) {
                    output_branches_.emplace_back(((TObjString*)obj)->GetString());
                }
            }
        }
    }

    if (gProofServ) {
        filename = filename + ".root";
        // send message to the client process
        const TString msg = TString::Format("SlaveBegin() of Ord = %s called.",
                                            gProofServ->GetOrdinal());
        gProofServ->SendAsynMessage(msg);
    } else {
        std::cout << "SalveBegin() called. (PROOF OFF) " << std::endl;
        return;
    }

    if (corrector_) {
        delete corrector_;
        corrector_ = nullptr;
    }
    // create an instance of VANDLEToFCorrector
    corrector_ = new VANDLEToFCorrector(vandle_corrector_config_);

    if (fProofFile) {
        delete fProofFile;
        fProofFile = nullptr;
    }
    // open a TProofOutputFile
    std::string fullname = proof_output_location_ + filename;
    fProofFile = new TProofOutputFile(fullname.c_str(), "");
    fProofFile->SetOutputFileName(fullname.c_str());

    if (fOutputFile) {
        delete fOutputFile;
        fOutputFile = nullptr;
    }
    TDirectory* savedir = gDirectory;
    fOutputFile = fProofFile->OpenFile("RECREATE");
    if (!fOutputFile)
        std::cout << "Failed to open output file. " << std::endl;
    std::cout << "File opened at " << fProofFile->GetDir() << fProofFile->GetFileName() << std::endl;
	 fOutputFile->SetCompressionSettings(ROOT::CompressionSettings(ROOT::kLZMA, 8));

    if (fOutputTree) {
        delete fOutputTree;
        fOutputTree = nullptr;
    }
    // initialize tree
    fOutputTree = new TTree("mergedCorrectedBeta", "mergedCorrectedBeta");
    fOutputTree->Branch("corrected_vandle_vec", "std::vector<CorrectedVANDLEData>", &corrected_vandle_vec_);
    fOutputTree->SetDirectory(fOutputFile);
    fOutputTree->AutoSave();
    gDirectory = savedir;

    return;
}

void CorrectionSelector::Init(TTree* mergedData) {
    tree_reader_.SetTree(mergedData);
    SetBranch();
    return;
}

void CorrectionSelector::SetBranch() {

    auto list = tree_reader_.GetTree()->GetListOfBranches();
    for (const auto& br_name : output_branches_) {
        auto br = (TBranch*)list->FindObject(br_name.c_str());
        // Create a branch named br_name if it does not exist
        if (!fOutputTree->FindBranch(br->GetName())) {
            TClass* tclass = (TClass*)gROOT->GetListOfClasses()->FindObject(br->GetClassName());
            auto addr = tclass->New();                                     //new instance of the class object filled in the branch
            br->SetAddress(addr);                                          // SetBranchAddress to the input tree
            fOutputTree->Branch(br->GetName(), br->GetClassName(), addr);  // Branch to the output tree
            std::cout << "SetBranchAddress(" << br->GetName() << "," << br->GetClassName() << "," << addr << ")" << std::endl;
        }
        else {
            // If the branch already exists, SetAddress to the new input tree
            br->SetAddress(fOutputTree->FindBranch(br_name.c_str())->GetAddress());
        }

    }

    // Somehow, tclass->New() doesn't work for OutputTreeData
    // temporaly set branch to the output tree for the mergedBeta branch
    if (!fBetaBranch)
        fBetaBranch = fOutputTree->Branch("mergedBeta", "OutputTreeData<PspmtData,PutputTreeData<PspmtData,TreeData>>", &beta_data_);
    return;
}

Bool_t CorrectionSelector::Process(Long64_t entry) {
    corrected_vandle_vec_.clear();

    tree_reader_.SetLocalEntry(entry);
    {
        auto beta = beta_.Get();
        auto clover_vec = clover_vec_.Get();
        auto gamma_scint_vec = gammascint_vec_.Get();
        auto vandle_vec = vandle_vec_.Get();
        for (auto const& vandle : *vandle_vec) {
            CorrectedVANDLEData data(vandle);
            // set vandle.qdc + 1000 as a test
            data.SetTestData(vandle.qdc + 1000);
            // set corrected tof
				const Double_t cortof = corrector_->CorrectToF(*beta, vandle, data);
            data.SetCorrectedTof(cortof);
				// set neutron energy
				const Double_t v = corrector_->GetIdealFL() * 1.e-2 / (cortof * 1.e-9);
				const Double_t b = v * v / (kSpeedOfLight * kSpeedOfLight);
				const Double_t g = 1. / sqrt(1. - b);
				const Double_t E = kNeutronMassMeV * (g - 1.);
				data.SetNeutronEnergy(E);
            corrected_vandle_vec_.push_back(data);
        }
        beta_data_ = *beta;
        fOutputTree->Fill();
    }  //end loop through the mergedData TTree

    return kTRUE;
}

void CorrectionSelector::SlaveTerminate() {
    // wirte the output file then add it to fOutput for merging
    std::cout << "[CorrectionSelector::SlaveTerminate()]: called. " << std::endl;
    if (gProofServ) {
        if (fOutputTree) {
            std::cout << "fOutputTree::GetEntries() " << fOutputTree->GetEntries() << std::endl;
            if (fOutputFile) {
                TDirectory* savedir = gDirectory;
                fOutputFile->cd();
                fOutputTree->Write();
                fProofFile->Print();
                //fOutput->Add(fProofFile);
                fOutputTree->SetDirectory(0);
                gDirectory = savedir;
                std::cout << "File written " << fProofFile->GetDir() << fProofFile->GetFileName() << std::endl;
                fOutputFile->Close();
            }
        }
    }
	 tree_reader_.SetTree((TTree*)nullptr);
    return;
}

void CorrectionSelector::Terminate() {
    std::cout << "[CorrectionSelector::Terminate()]: called. " << std::endl;
    if (!use_proof_) {
        // If it was not a proof session, write output file here.
        if (fOutputTree) {
            std::cout << "fOutputTree::GetEntries() " << fOutputTree->GetEntries() << std::endl;
            fOutputTree->Write();
        }
        if (fOutputFile) {
            fOutputFile->Close();
        }
    } else {
    }

    return;
}
