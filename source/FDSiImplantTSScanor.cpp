/** FDSiImplantTSScanor.cpp generated by R. Yokoyama 09/06/2018 **/
#include "FDSiImplantTSScanor.hpp"

const std::string FDSiImplantTSScanor::kMsgPrefix("[FDSiImplantTSScanor]:");

void FDSiImplantTSScanor::Configure(const std::string& yaml_node_name) {
    TSScannorBase<PspmtData>::Configure(yaml_node_name);
    pid_data_ = new TTreeReaderValue<std::vector<processor_struct::PID>>(*tree_reader_,"pid_vec_");
    YamlReader yaml_reader(yaml_node_name);
    rit_threshold_ = yaml_reader_->GetDouble("RITThreshold",false,0);
    pin0_threshold_ = yaml_reader_->GetDouble("PIN0Threshold",false,0);
    tac1_threshold_ = yaml_reader_->GetDouble("TAC1Threshold",false,0);
    return;
}

Bool_t FDSiImplantTSScanor::IsInGate()
{
    if(!ImplantTSScannor::IsInGate())
        return false;

    if (!pid_cut_)
        return true;

    {   /* checks if there is a coincidence with veto detector */
        const Double_t energy_first = tree_data_->Get()->rit_b1_.energy_;
        const Double_t energy_second = tree_data_->Get()->rit_b2_.energy_;
        if( energy_first > rit_threshold_ || energy_second > rit_threshold_ )
            return false;
    }
    auto pid_vec = pid_data_->Get();
    if (pid_vec->empty())
        return false;
    const Double_t pin0 = pid_vec->at(0).pin_0_energy;
    const Double_t tac1 = pid_vec->at(0).tac_1;
    if ( pin0 < pin0_threshold_ || tac1 < tac1_threshold_ )
       return false;

    //if (pid_cut_->IsInside(pid_vec->at(0).pin_0_energy, pid_vec->at(0).tof1))
    if (pid_cut_->IsInside(tac1, pin0))
        return true;
    else
        return false;

}
