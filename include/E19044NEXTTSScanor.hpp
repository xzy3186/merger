/** E19044NEXTTSScanor.hpp generated by R. Yokoyama 10/06/2020 **/

#ifndef E19044_NEXT_TSSCANOR_HPP_
#define E19044_NEXT_TSSCANOR_HPP_

#include "TSScannorBase.hpp"
#include "PaassRootStruct.hpp"
#include "E19044Data.hpp"

/** timestamp scannor class for next events **/
class E19044NEXTTSScanor : public TSScannorBase<PixTreeEvent>
{
public:
    const static std::string kMsgPrefix;
    E19044NEXTTSScanor(){}
    ~E19044NEXTTSScanor(){}
    void SetReader();

protected:
    /** energy gates on PSPMT dynode**/

    ULong64_t GetTS() const
    {
        if(!tree_data_) throw kMsgPrefix + "In GetTS(), tree_data_ is null";
        if(!tree_data_->Get()) throw kMsgPrefix + "In GetTS(), tree_data_->Get() returned null";
        return tree_data_->Get()->externalTS1;
        //return tree_data_->Get()->time_;
    }

    Bool_t IsInGate(); // gate conditions
};

/* timestamp scanor class for VANDLE merged beta events */
class E19044MergedVANDLETSScanor : public TSScannorBase<E19044Output>
{
public:
    const static std::string kMsgPrefix;
    E19044MergedVANDLETSScanor(){}
    ~E19044MergedVANDLETSScanor(){}
    void SetReader();

protected:
    /** energy gates on PSPMT dynode**/

    ULong64_t GetTS() const
    {
        if(!tree_data_) throw kMsgPrefix + "In GetTS(), tree_data_ is null";
        if(!tree_data_->Get()) throw kMsgPrefix + "In GetTS(), tree_data_->Get() returned null";
        return tree_data_->Get()->input_.external_ts_high_;
    }

    Bool_t IsInGate(); // gate conditions
};

#endif /* E19044_NEXT_TSSCANOR_HPP_ */
