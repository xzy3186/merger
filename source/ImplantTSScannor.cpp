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
    return true;
    auto pspmt_low = tree_data_->Get()->low_gain_;
    /****
    auto pspmt_high = tree_data_->Get()->high_gain_;
    if(pspmt_low.xa_<10 || pspmt_low.xb_<10 || pspmt_low.ya_<10 || pspmt_low.yb_<10 )
        return false;
    if(pspmt_low.xa_>30000 || pspmt_low.xb_>30000 || pspmt_low.ya_>30000 || pspmt_low.yb_>30000)
        return false;*///
    if(pspmt_low.energy_ < 2000 || pspmt_low.energy_ > 8000 )
        return false;

    if(!pspmt_low.valid_)
        return false;

    // Gating on de si
    const Double_t tdiff_top = tree_data_->Get()->desi_top_time_ - pspmt_low.time_ + 95.;
    const Double_t de_top = tree_data_->Get()->desi_top_energy_;
    const Double_t tdiff_bottom = tree_data_->Get()->desi_bottom_time_ - pspmt_low.time_ + 95.;
    const Double_t de_bottom = tree_data_->Get()->desi_bottom_energy_;

    if( tdiff_top<5. && tdiff_top>-5. && de_top>5200. && de_top<5600. ) //83Ga
        return true;
    else if ( tdiff_bottom<5. && tdiff_bottom>-5. && de_bottom>5600. && de_bottom<6400. ) //83Ga
        return true;
    else
        return false;
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