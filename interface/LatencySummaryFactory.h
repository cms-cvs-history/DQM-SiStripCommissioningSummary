#ifndef DQM_SiStripCommissioningSummary_LatencySummaryFactory_H
#define DQM_SiStripCommissioningSummary_LatencySummaryFactory_H

#include "DQM/SiStripCommissioningSummary/interface/SummaryHistogramFactory.h"
#include "DQM/SiStripCommissioningAnalysis/interface/LatencyAnalysis.h"

class SummaryGenerator;

template<>
class SummaryHistogramFactory<LatencyAnalysis> {
  
 public:
  
  SummaryHistogramFactory();
  ~SummaryHistogramFactory();

  void init( const sistrip::Monitorable&, 
	     const sistrip::Presentation&,
	     const sistrip::View&, 
	     const std::string& top_level_dir, 
	     const sistrip::Granularity& );
  
  uint32_t extract( const std::map<uint32_t,LatencyAnalysis>& data );
  
  void fill( TH1& summary_histo );
  
 private:
  
  sistrip::Monitorable mon_;
  sistrip::Presentation pres_;
  sistrip::View view_;
  std::string level_;
  sistrip::Granularity gran_;
  SummaryGenerator* generator_;
  
};

#endif // DQM_SiStripCommissioningSummary_LatencySummaryFactory_H
