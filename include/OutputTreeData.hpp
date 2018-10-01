#ifndef VANDLE_MERGER_OUTPUTTREEDATA_HPP_
#define VANDLE_MERGER_OUTPUTTREEDATA_HPP_

#include <vector>
#include <TObject.h>
#include <TClonesArray.h>
#include "ProcessorRootStruc.hpp"

template <class T, class U>
class OutputTreeData : public T
{
public:
    TClonesArray output_array_;

    OutputTreeData(){}
    OutputTreeData( const T &input_event, const TClonesArray &clones_array ) : T(input_event), output_array_(clones_array){}
    OutputTreeData( const OutputTreeData &obj ) : T(obj), output_array_(obj.output_array_){}
    virtual ~OutputTreeData(){}

    void Clear()
    {
        output_array_.Clear();
        T::Clear();
    }
    ClassDef(OutputTreeData,1)
};

#endif /** VANDLE_MERGER_OUTPUTTREEDATA_HPP_ **/
