#include "FDSiBetaTreeMerger.hpp"

bool FDSiBetaTreeMerger::IsInGate(const PspmtData& in1, const PspmtData& in2) {

	const auto pspmt_imp = in2.low_gain_;
	const auto pspmt_beta = in1.high_gain_;
   /* if high gain position is available, use it for correlation */
   if(pspmt_beta.valid_) {
       const double x = pspmt_beta.pos_x_ - pspmt_imp.pos_x_;
       const double y = pspmt_beta.pos_y_ - pspmt_imp.pos_y_;

       if((x*x + y*y) < (correlation_radius_*correlation_radius_))
           return true;
       else
           return false;
   }
   /* otherwise, use low gain position */
   else if (in1.low_gain_.valid_) {
       const double x = in1.low_gain_.pos_x_ - pspmt_imp.pos_x_;
       const double y = in1.low_gain_.pos_y_ - pspmt_imp.pos_y_;

       if((x*x + y*y) < (correlation_radius_*correlation_radius_))
           return true;
       else
           return false;
   }
   return false;
}
