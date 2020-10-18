/** ImplantTSScannor.cpp generated by R. Yokoyama 09/06/2018 **/
#include "ImplantTSScannor.hpp"

const std::string ImplantTSScannor::kMsgPrefix("[ImplantTSScannor]:");

void ImplantTSScannor::SetReader()
{
    TSScannorBase<PspmtData>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<PspmtData>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

    low_gain_min_ = yaml_reader_->GetDouble("MinLowGainDynEnergy");
    low_gain_max_ = yaml_reader_->GetDouble("MaxLowGainDynEnergy");
    std::cout << kMsgPrefix << "Implant range on dynode low gain: " << low_gain_min_ << " - " << low_gain_max_ << std::endl;
    return;
}

Bool_t ImplantTSScannor::IsInGate()
{

    auto pspmt_low = tree_data_->Get()->low_gain_;
    if(!pspmt_low.valid_)
        return false;

	 // Energy gate on pspmt low energy
    if(pspmt_low.energy_ < low_gain_min_ || pspmt_low.energy_ > low_gain_max_ )
        return false;

    // Anti-gating on veto
    //{
    //    if(tree_data_->Get()->veto_first_.energy_>10. && tree_data_->Get()->veto_second_.energy_>10.)
    //        return false;
    //}

    return true;
}

const std::string MergedImplantTSScannor::kMsgPrefix("[MergedImplantTSScannor]:");

void MergedImplantTSScannor::SetReader()
{
    TSScannorBase<OutputTreeData<PspmtData, TreeData>>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<OutputTreeData<PspmtData, TreeData>>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

    return;
}

Bool_t MergedImplantTSScannor::IsInGate()
{
    if(tree_data_->Get()->output_vec_.size()==0)
        return false;
    else
        return true;
}