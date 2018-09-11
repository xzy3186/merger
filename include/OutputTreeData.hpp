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
    ClassDef(OutputTreeData,1)
};

#endif /** VANDLE_MERGER_OUTPUTTREEDATA_HPP_ **/
