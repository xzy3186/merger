#ifndef VANDLE_MERGER_OUTPUTTREEDATA_HPP_
#define VANDLE_MERGER_OUTPUTTREEDATA_HPP_

#include <vector>
#include <TObject.h>
#include "PaassRootStruct.hpp"

template <class T, class U>
class OutputTreeData
{
public:
    T input_;
    std::vector<U> output_vec_;

	 OutputTreeData() {}
    OutputTreeData( const T &input_event ){input_ = input_event;}
	 OutputTreeData(const OutputTreeData<T, U>& obj) {
		 output_vec_ = obj.output_vec_;
         input_ = obj.input_;
	 }
    virtual ~OutputTreeData(){}

    void Clear()
    {
        output_vec_.clear();
        input_.Clear();
    }
    ClassDef(OutputTreeData,4)
};

// template <class T, class U>
// class NEXTOutputTreeData : public T
// {
// public:
//     std::vector<U> next_output_vec_;

// 	 NEXTOutputTreeData():T() {}
//     NEXTOutputTreeData( const T &input_event ):T(input_event){}
// 	 NEXTOutputTreeData(const NEXTOutputTreeData<T, U>& obj) :T(obj) {
// 		 next_output_vec_ = obj.next_output_vec_;
// 	 }
//     virtual ~NEXTOutputTreeData(){}

//     void Clear()
//     {
//         next_output_vec_.clear();
//         T::Clear();
//     }
//     ClassDef(NEXTOutputTreeData,4)
// };

#endif /** VANDLE_MERGER_OUTPUTTREEDATA_HPP_ **/
