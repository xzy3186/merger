#include "BetaEventTag.h"

void BetaEventTag::Configure() {
	YamlReader yaml_reader("BetaEventTag");
	auto file_path = yaml_reader.GetString("FilePath");
	auto nuclides = yaml_reader.GetNode("Nuclides");
	for (const auto &nuclide : nuclides) {
		auto nucl = nuclide.as<std::string>();
		std::string fname = file_path + "eventList_" + nucl + ".root";
		std::string name = "mergedCorrectedBeta_" + nucl + "_r0.2.root";
		TFile f(fname.c_str());
		TBetaEventList* obj = (TBetaEventList*)f.Get(name.c_str());
		obj->SetName(nucl.c_str());
		std::cout << "Read object: " << obj->GetName() << std::endl;
		lists_.push_back(*obj);
	}

	return;
}

std::string BetaEventTag::GetTag(const std::string& fname, const ULong64_t& id) {
	std::string tag = "";
	for (auto& list : lists_) {
		if (list.IsIntheList(fname, id)) {
			tag = tag + list.GetName();
		}
	}
	return tag;
}
