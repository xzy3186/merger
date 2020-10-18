#include "E19044NEXTTSScanor.hpp"

const std::string E19044NEXTTSScanor::kMsgPrefix("[E19044NEXTTSScannor]:");

void E19044NEXTTSScanor::SetReader() {
    
    TSScannorBase<PixTreeEvent>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<PixTreeEvent>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

}

Bool_t E19044NEXTTSScanor::IsInGate() {
	return true;
}

const std::string E19044MergedVANDLETSScanor::kMsgPrefix("[E19044MergedVANDLETSScannor]:");

void E19044MergedVANDLETSScanor::SetReader() {

    TSScannorBase<E19044Output>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<E19044Output>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

}

Bool_t E19044MergedVANDLETSScanor::IsInGate() {
	return true;
}