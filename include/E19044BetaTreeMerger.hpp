#ifndef E19044_BETA_TREE_MERGER_HPP
#define E19044_BETA_TREE_MERGER_HPP
#include "OutputTreeData.hpp"
#include "PspmtData.hpp"
#include "BetaTreeMerger.hpp"
#include "ImplantTSScannor.hpp"
#include "E19044BetaTSScanor.hpp"
#include "E19044Data.hpp"

class E19044BetaTreeMerger : public BetaTreeMerger<E19044Output, PspmtData, PspmtData> {
public:
	E19044BetaTreeMerger(ImplantTSScannor* in1, E19044BetaTSScanor* in2) : BetaTreeMerger<E19044Output,PspmtData,PspmtData>(in1, in2) {};
	virtual ~E19044BetaTreeMerger() {};

	bool IsInGate(const PspmtData& in1, const PspmtData& in2) override;
};
#endif /* E19044_BETA_TREE_MERGER_HPP */