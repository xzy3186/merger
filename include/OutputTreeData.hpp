#ifndef VANDLE_MERGER_OUTPUTTREEDATA_HPP_
#define VANDLE_MERGER_OUTPUTTREEDATA_HPP_

#include "BigRIPSTreeData.h"
#include "ProcessorRootStruc.hpp"

template <class T, class U>
class OutputTreeData : public T
{
public:
    std::vector<U> output_vec_;

    OutputTreeData(){}
    OutputTreeData( const T &input_event ):T(input_event){}
    virtual ~OutputTreeData(){}

    void Clear()
    {
        output_vec_.clear();
        T::Clear();
    }
};


///** Implant event data structure merged with BigRIPS events **/
//class ImplantTreeData : public: PixTreeEvent
//{
//public:
//    std::vector<TreeData> bigrips_vec_;
//
//    ImplantTreeData(){}
//    ImplantTreeData( const PixTreeEvent &implant_event ):PixTreeEvent(implant_event){}
//    virtual ~ImplantTreeData(){}
//
//    void Clear()
//    {
//        bigrips_vec_.clear();
//        PixTreeEvent::Clear();
//    }
//};
//
///** Beta event data structure merged with Implant events **/
//class BetaTreeData : public: PixTreeEvent
//{
//public:
//    std::vector<ImplantTreeData> implant_vec_;
//
//    BetaTreeData(){}
//    BetaTreeData( const PixTreeEvent &beta_event ):PixTreeEvent(beta_event){}
//    virtual ~BetaTreeData(){}
//
//    void Clear()
//    {
//        implant_vec_.clear();
//        PixTreeEvent::Clear();
//    }
//};

#endif /** VANDLE_MERGER_OUTPUTTREEDATA_HPP_ **/
