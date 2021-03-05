#ifndef PSPMT_ANALYZER_HPP
#define PSPMT_ANALYZER_HPP
#include <fstream>
#include <TTree.h>
#include "PaassRootStruct.hpp"
#include "PspmtAnalyzerData.hpp"
#include "PspmtData.hpp"
#include "TraceAnalyzer.hpp"
#include "YamlReader.hpp"
#include <TH1.h>
#include <TF1.h>
#include "YSOPixelatedMap.hpp"

class PspmtAnalyzer {
public:
   const static std::string kMsgPrefix;

   PspmtAnalyzer(){};
   PspmtAnalyzer(const std::string &yaml_node_name){
      Configure(yaml_node_name);
   }
   virtual ~PspmtAnalyzer() {
      if(yso_map_)
         delete yso_map_;
      yso_map_ = nullptr;
   };

   virtual int Configure(const std::string &yaml_node_name);
   virtual int ResetConfig();
   virtual int Begin();
   virtual int Process(std::vector<processor_struct::PSPMT> &pspmt_vec, const ULong64_t ts);
   virtual int Terminate();

   virtual void SetEventData(PixTreeEvent* pixie_event);
   virtual void CalculatePositionH(pspmt_data_struc &data);
   virtual void CalculatePositionL(pspmt_data_struc &data);
   void Correct(double &val, TF1* func);

protected:
   TTree *output_tree_ = nullptr;
   PspmtAnalyzerData data_;
   PspmtData pspmt_data_;
   event_info_struc event_info_;
   std::vector<processor_struct::BATO> bato_data_;
   std::vector<processor_struct::CLOVER> clover_data_;
   std::vector<processor_struct::DOUBLEBETA> doublebeta_data_;
   std::vector<processor_struct::GAMMASCINT> gammascint_data_;
   std::vector<processor_struct::LOGIC> logic_data_;
   std::vector<processor_struct::NEXT> next_data_;
   std::vector<processor_struct::PID> pid_data_;
   std::vector<processor_struct::ROOTDEV> rootdev_data_;
   std::vector<processor_struct::SINGLEBETA> singlebeta_data_;
   std::vector<processor_struct::VANDLE> vandle_data_;

   /* time window parameters relative to the high gain dynode signal */
   /* in clock ticks (8ns) */
   Double_t kTWINDOW; 
   Double_t kTOFFSET; 
   Double_t kTWINDOW_ION; 
   Double_t kTOFFSET_ION; 
   Double_t kTWINDOW_VETO; 
   Double_t kTOFFSET_VETO; 
   /** energy thresholds and overflow thresholds for anode signals **/
   Double_t kHIGH_GAIN_THRESHOLD;
   Double_t kHIGH_GAIN_OVERFLOW;
   Double_t kLOW_GAIN_THRESHOLD;
   Double_t kLOW_GAIN_OVERFLOW;
   /** offset for trace_energy_ **/
   TF1* fCorrectionHighGainXA = nullptr;
   TF1* fCorrectionHighGainXB = nullptr;
   TF1* fCorrectionHighGainYA = nullptr;
   TF1* fCorrectionHighGainYB = nullptr;
   TF1* fCorrectionLowGainXA = nullptr;
   TF1* fCorrectionLowGainXB = nullptr;
   TF1* fCorrectionLowGainYA = nullptr;
   TF1* fCorrectionLowGainYB = nullptr;
   TF1* fCorrectionHighGainPosX = nullptr;
   TF1* fCorrectionHighGainPosY = nullptr;
   TF1* fCorrectionLowGainPosX = nullptr;
   TF1* fCorrectionLowGainPosY = nullptr;

   YSOPixelatedMap* yso_map_ = nullptr;

};

const std::string TraceAnalyzer::kMsgPrefix("[TraceAnalyzer:]");
const std::string PspmtAnalyzer::kMsgPrefix("[PspmtAnalyzer:]");

#endif
