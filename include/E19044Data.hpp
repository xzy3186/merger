///@file E19044Data.hpp
///@brief Type definition of the merged output data for E19044 exp.
///@author R. Yokoyama
///@date October, 16 2020

#ifndef E19044_DATA_HPP_
#define E19044_DATA_HPP_

#include "PaassRootStruct.hpp"
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"

typedef OutputTreeData<PspmtData, PspmtData> E19044Output;
typedef OutputTreeData<E19044Output, PixTreeEvent> NEXTOutput;

#endif /* E19044_DATA_HPP_ */