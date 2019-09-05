// Corrected VANDLE data class created by Rin Yokoyama on 8/30/2019

#ifndef CORRECTED_VANDLE_DATA_H
#define CORRECTED_VANDLE_DATA_H

#include <iostream>
#include "ProcessorRootStruc.hpp"

class CorrectedVANDLEData : public TObject {
public:
	CorrectedVANDLEData() {}
	CorrectedVANDLEData(const processor_struct::VANDLES& vandle_data) {
		vandle_data_ = vandle_data;
	}
	virtual ~CorrectedVANDLEData() {};
	void SetTestData(double test_data) { test_data_ = test_data; }

protected:
	processor_struct::VANDLES vandle_data_;
	double test_data_;

	ClassDef(CorrectedVANDLEData,1)
};

#endif
