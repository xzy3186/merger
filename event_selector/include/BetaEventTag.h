#ifndef BETA_EVENT_TAG_H
#define BETA_EVENT_TAG_H

#include <TFile.h>
#include <TKey.h>
#include "TBetaEventList.h"
#include "YamlReader.hpp"

class BetaEventTag {
public:
	BetaEventTag() { Configure(); }
	virtual ~BetaEventTag() {}

	void Configure();
	std::string GetTag(const std::string &fname, const ULong64_t &id);
protected:
	std::vector<TBetaEventList> lists_;
};

#endif