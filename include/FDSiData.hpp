///@file FDSiData.hpp
///@brief Type definition of the merged output data for FDSi exp.
///@author R. Yokoyama
///@date October, 16 2020

#ifndef FDSi_DATA_HPP_
#define FDSi_DATA_HPP_

#include "PaassRootStruct.hpp"
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"

typedef OutputTreeData<PspmtData, PspmtData> FDSiOutput;
typedef OutputTreeData<FDSiOutput, PixTreeEvent> VandleOutput;
typedef OutputTreeData<PixTreeEvent, PixTreeEvent> FDSiTestOutput;

#endif /* FDSi_DATA_HPP_ */
