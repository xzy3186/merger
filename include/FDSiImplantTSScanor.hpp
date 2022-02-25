/** FDSiImplantTSScanor.hpp generated by R. Yokoyama 10/25/2020 **/

#ifndef FDSi_IMPLANTTSSCANOR_HPP_
#define FDSi_IMPLANTTSSCANOR_HPP_

#include "ImplantTSScannor.hpp"
#include <TFile.h>
#include <TCutG.h>
#include <TTreeReaderValue.h>
#include "PaassRootStruct.hpp"

/** timestamp scannor class for implant events **/
class FDSiImplantTSScanor : public ImplantTSScannor
{
public:
    const static std::string kMsgPrefix;
    FDSiImplantTSScanor(){}
    virtual ~FDSiImplantTSScanor(){}
    void Configure(const std::string& yaml_node_name);
    void SetPidCut(TCutG* cut) { pid_cut_ = cut; }

protected:
    /** PID gate **/
    TCutG* pid_cut_ = nullptr;
    TTreeReaderValue<std::vector<processor_struct::PID>>* pid_data_;
    Double_t rit_threshold_;
    Double_t pin0_threshold_;
    Double_t tac1_threshold_;

    Bool_t IsInGate() override; // gate conditions
};

#endif /* FDSi_IMPLANTTSSCANOR_HPP_ */