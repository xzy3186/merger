#include "FDSiVANDLETSScanor.hpp"

const std::string FDSiVANDLETSScanor::kMsgPrefix("[FDSiVANDLETSScannor]:");

void FDSiVANDLETSScanor::SetReader() {
    
    TSScannorBase<PixTreeEvent>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<PixTreeEvent>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

}

Bool_t FDSiVANDLETSScanor::IsInGate() {
	return true;
}

//const std::string E19044MergedVANDLETSScanor::kMsgPrefix("[E19044MergedVANDLETSScannor]:");
//
//void E19044MergedVANDLETSScanor::SetReader() {
//
//    TSScannorBase<E19044Output>::SetReader();
//    std::string br_name = yaml_reader_->GetString("PixieBranchName");
//    tree_data_ = new TTreeReaderValue<E19044Output>(*tree_reader_,br_name.c_str());
//    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;
//
//}
//
//Bool_t E19044MergedVANDLETSScanor::IsInGate() {
//	return true;
//}
