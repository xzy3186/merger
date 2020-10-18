/** E19044BetaTSScanor.cpp generated by R. Yokoyama 10/06/2020 **/
#include "E19044BetaTSScanor.hpp"

const std::string E19044BetaTSScanor::kMsgPrefix("[E19044BetaTSScanor]:");

void E19044BetaTSScanor::SetReader()
{
    TSScannorBase<PspmtData>::SetReader();
    std::string br_name = yaml_reader_->GetString("PixieBranchName");
    tree_data_ = new TTreeReaderValue<PspmtData>(*tree_reader_,br_name.c_str());
    std::cout << kMsgPrefix << "TTreeReaderValue: " << br_name << " created." << std::endl;

    high_gain_min_ = yaml_reader_->GetDouble("MinHighGainDynEnergy");
    high_gain_max_ = yaml_reader_->GetDouble("MaxHighGainDynEnergy");
    std::cout << kMsgPrefix << "Beta range on dynode high gain: " << high_gain_min_ << " - " << high_gain_max_ << std::endl;
    low_gain_min_ = yaml_reader_->GetDouble("MinLowGainDynEnergy");
    low_gain_max_ = yaml_reader_->GetDouble("MaxLowGainDynEnergy");
    std::cout << kMsgPrefix << "Beta range on dynode low gain: " << low_gain_min_ << " - " << low_gain_max_ << std::endl;
    return;
}

Bool_t E19044BetaTSScanor::IsInGate()
{

    auto high = tree_data_->Get()->high_gain_;


    {   /* checks if there is a coincidence with veto detector */
        const Double_t energy_first = tree_data_->Get()->veto_first_.energy_;
        const Double_t energy_second = tree_data_->Get()->veto_second_.energy_;
        if( energy_first > 10. && energy_second > 10. )
            return false;
    }

    {   /* energy gates on pspmt */
        auto pspmt_high = tree_data_->Get()->high_gain_;
        auto pspmt_low = tree_data_->Get()->low_gain_;
        if (pspmt_high.energy_<high_gain_min_ || pspmt_high.energy_>high_gain_max_)
           return false;
        if (pspmt_low.energy_<low_gain_min_ || pspmt_low.energy_>low_gain_max_)
           return false;
        if(pspmt_high.valid_ == 0 )
           return false;
    }

    return true;
}

