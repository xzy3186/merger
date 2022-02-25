#ifndef FDSi_BETA_TREE_MERGER_HPP
#define FDSi_BETA_TREE_MERGER_HPP
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"
#include "BetaTreeMerger.hpp"
#include "ImplantTSScannor.hpp"
#include "FDSiBetaTSScanor.hpp"
#include "FDSiData.hpp"

class FDSiBetaTreeMerger : public BetaTreeMerger<FDSiOutput, PspmtData, PspmtData> {
public:
	FDSiBetaTreeMerger(FDSiBetaTSScanor* in1,ImplantTSScannor* in2) : BetaTreeMerger<FDSiOutput,PspmtData,PspmtData>(in1, in2) {};
	virtual ~FDSiBetaTreeMerger() {};

	bool IsInGate(const PspmtData& in1, const PspmtData& in2) override;
};
#endif /* FDSi_BETA_TREE_MERGER_HPP */
