#ifndef PSPMT_ANALYZER_HPP
#define PSPMT_ANALYZER_HPP
#include <fstream>
#include <TTree.h>
#include "ProcessorRootStruc.hpp"
#include "PspmtAnalyzerData.hpp"
#include "PspmtData.hpp"
#include "TraceAnalyzer.hpp"
#include "YamlReader.hpp"
#include <TH1.h>

struct event_info_struc {
   TString file_name_;
   ULong64_t pixie_event_num_;
};

class PspmtAnalyzer {
public:
   const static std::string kMsgPrefix;

   PspmtAnalyzer(){};
   PspmtAnalyzer(const std::string &yaml_node_name){
      Configure(yaml_node_name);
   }
   virtual ~PspmtAnalyzer(){};

   virtual int Configure(const std::string &yaml_node_name);
   virtual int Begin();
   virtual int Process(std::vector<processor_struct::PSPMT> &pspmt_vec, const ULong64_t ts);
   virtual int Terminate();

   void SetEventId(const TString &file_name, const ULong64_t event_num);
   void SetEventData(PixTreeEvent* pixie_event);
   void CalculatePositionH(pspmt_data_struc &data);
   void CalculatePositionL(pspmt_data_struc &data);

protected:
   std::fstream pos;
   TTree *output_tree_;
   PspmtAnalyzerData data_;
   PspmtData pspmt_data_;
   event_info_struc event_info_;
   std::vector<processor_struct::CLOVERS> clover_data_;
   std::vector<processor_struct::VANDLES> vandle_data_;
   std::vector<processor_struct::GAMMASCINT> gamma_scint_data_;
   std::vector<processor_struct::DOUBLEBETA> double_beta_data_;

   /* time window parameters relative to the high gain dynode signal */
   /* in clock ticks (8ns) */
   Double_t kTWINDOW; 
   Double_t kTOFFSET; 
   Double_t kTWINDOW_DESI; 
   Double_t kTOFFSET_DESI; 
   Double_t kTWINDOW_ION; 
   Double_t kTOFFSET_ION; 
   Double_t kTWINDOW_VETO; 
   Double_t kTOFFSET_VETO; 
   Double_t kTWINDOW_F11;
   Double_t kTOFFSET_F11;
   /** energy thresholds and overflow thresholds for anode signals **/
   Double_t kHIGH_GAIN_THRESHOLD;
   Double_t kHIGH_GAIN_OVERFLOW;
   Double_t kLOW_GAIN_THRESHOLD;
   Double_t kLOW_GAIN_OVERFLOW;
   /** offset for trace_energy_ **/
   Double_t kHIGH_GAIN_OFFSET_XA;
   Double_t kHIGH_GAIN_OFFSET_XB;
   Double_t kHIGH_GAIN_OFFSET_YA;
   Double_t kHIGH_GAIN_OFFSET_YB;
   Double_t kLOW_GAIN_OFFSET_XA;
   Double_t kLOW_GAIN_OFFSET_XB;
   Double_t kLOW_GAIN_OFFSET_YA;
   Double_t kLOW_GAIN_OFFSET_YB;

};

const std::string TraceAnalyzer::kMsgPrefix("[TraceAnalyzer:]");
const std::string PspmtAnalyzer::kMsgPrefix("[PspmtAnalyzer:]");

#endif
