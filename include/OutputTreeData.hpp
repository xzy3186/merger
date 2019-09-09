#ifndef VANDLE_MERGER_OUTPUTTREEDATA_HPP_
#define VANDLE_MERGER_OUTPUTTREEDATA_HPP_

#include <vector>
#include <TObject.h>
#include "ProcessorRootStruc.hpp"

template <class T, class U>
class OutputTreeData : public T
{
public:
    std::vector<U> output_vec_;

	 OutputTreeData():T() {}
    OutputTreeData( const T &input_event ):T(input_event){}
	 OutputTreeData(const OutputTreeData<T, U>& obj) :T(obj) {
		 output_vec_ = obj.output_vec_;
	 }
    virtual ~OutputTreeData(){}

    void Clear()
    {
        output_vec_.clear();
        T::Clear();
    }
    ClassDef(OutputTreeData,4)
};

#endif /** VANDLE_MERGER_OUTPUTTREEDATA_HPP_ **/
