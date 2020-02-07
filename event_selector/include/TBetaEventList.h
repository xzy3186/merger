#ifndef TBETA_EVENT_LIST_H
#define TBETA_EVENT_LIST_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <TNamed.h>

class TBetaEventList : public TNamed {
public:
	TBetaEventList();
	virtual ~TBetaEventList();

	void AddEvent(const std::string& fname, const ULong64_t& id);
	bool IsIntheList(const std::string& fname, const ULong64_t& id) {
		auto it = map_.find(fname);
		if (it == map_.end())
			return false;
		auto it2 = std::find((*it).second.begin(), (*it).second.end(), id);
		if (it2 == (*it).second.end())
			return false;
		return true;
	}
	const std::map<std::string, std::vector<ULong64_t>>& GetMap() const { return map_; }

protected:
	std::map<std::string, std::vector<ULong64_t>> map_;

	ClassDef(TBetaEventList,1)
};

#endif