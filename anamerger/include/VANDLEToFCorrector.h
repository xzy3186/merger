// VANDLEToFCorrector created by Rin Yokoyama on 09/05/2019

#ifndef VANDLE_TOF_CORRECTOR_H
#define VANDLE_TOF_CORRECTOR_H

#include <iostream>
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"
#include "YamlReader.hpp"
#include "PspmtData.hpp"
#include "ProcessorRootStruc.hpp"
#include "CorrectedVANDLEData.h"

class VANDLEBar {
public:
	VANDLEBar(){}
	VANDLEBar(const int& bar_num, const double& z_zero, const double& angle, const double& lr_offset) :
		bar_num_(bar_num),
		z_zero_(z_zero),
		angle_(angle),
		lr_offset_(lr_offset) {}
	virtual ~VANDLEBar() {}

	double GetZZero() const { return z_zero_; }
	double GetAngle() const { return angle_; }
	double GetLROffset() const { return lr_offset_; }

protected:
	int bar_num_;
	double z_zero_;
	double angle_;
	double lr_offset_;
};

class VANDLEToFCorrector {
public:
	VANDLEToFCorrector() {}
	VANDLEToFCorrector(const std::string& config_file) {
		pspmt_x_correction_ = nullptr;
		pspmt_y_correction_ = nullptr;
		Configure(config_file);
	}
	virtual ~VANDLEToFCorrector() {}

	void Configure(const std::string& config_file);
	double CorrectToF(const PspmtData& pspmt_data, const processor_struct::VANDLES& vandle, CorrectedVANDLEData& correctedData) const;

protected:
	std::map<int, VANDLEBar> vandle_bar_map_;
	TF1* pspmt_x_correction_;
	TF1* pspmt_y_correction_;
	TF1* vandle_walk_correction_;
	double pspmt_rotation_angle_;
	double speed_of_light_medium_;
	double ideal_flight_path_;

	const TVector3* GetBetaPosition(const PspmtData& pspmt_data) const;
	const TVector3* GetVandlePosition(const processor_struct::VANDLES& vandle) const;
};

#endif