#include "TBetaEventList.h"

ClassImp(TBetaEventList)

TBetaEventList::TBetaEventList(){}

TBetaEventList::~TBetaEventList(){}

void TBetaEventList::AddEvent(const std::string &fname, const ULong64_t &id){
	auto it = map_.find(fname);
	if (it == map_.end()) {
		std::vector<ULong64_t> v = { id };
		map_.emplace(fname, v);
		return;
	}
	else {
		(*it).second.push_back(id);
		return;
	}
}