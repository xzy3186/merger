#ifndef DUMP_TREE_DATA_H
#define DUMP_TREE_DATA_H

#include "ProcessorRootStruc.hpp"

class DumpTreeData : public TObject {
public:

   DumpTreeData(){};
   virtual ~DumpTreeData(){};

   DumpTreeData(const DumpTreeData &obj):TObject(obj)
   {
      externalTS1 = obj.externalTS1;
      clover_vec_ = obj.clover_vec_;
      vandle_vec_ = obj.vandle_vec_;
      pspmt_vec_ = obj.pspmt_vec_;
      eventId = obj.eventId;
   }

   void Clear()
   {
      externalTS1 = 0;
      clover_vec_.clear();
      vandle_vec_.clear();
      pspmt_vec_.clear();
   }

   ULong64_t externalTS1;
   ULong64_t eventId;
   std::vector<processor_struct::CLOVERS> clover_vec_;
   std::vector<processor_struct::VANDLES> vandle_vec_;
   std::vector<processor_struct::PSPMT> pspmt_vec_;

   ClassDef(DumpTreeData,1)
};


#endif
