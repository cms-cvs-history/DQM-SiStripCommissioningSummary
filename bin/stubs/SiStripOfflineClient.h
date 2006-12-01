#ifndef DQM_SiStripCommissioningSummary_SiStripOfflineClient_H
#define DQM_SiStripCommissioningSummary_SiStripOfflineClient_H

#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"
#include <boost/cstdint.hpp>
#include <string>

class TH1;

/**
   @class SiStripOfflineClient
   @author M.Wingham, R.Bainbridge
   
   @brief Class which reads TProfile histograms from a "commissioning
   client" root file, performs an analysis to extract the
   "monitorables" and creates a summary histogram.
*/
class SiStripOfflineClient {
  
 public:
  
  typedef std::vector<TH1*> Histos;
  typedef std::map< uint32_t, Histos > HistosMap;
  
  SiStripOfflineClient( const std::string& filename,
			const sistrip::SummaryHisto&,
			const sistrip::SummaryType&,
			const std::string& top_level_dir,
			const sistrip::Granularity& );
  ~SiStripOfflineClient();
  
  /** Performs analysis on each commissioning histogram found in the
      input root file and saves monitorables in map/histo. */
  void analysis();
  
 private: // ---------- private methods ----------
  
  /** Sets run number based on name of input root file. */
  void setRunNumber() {;}
  
  /** Fills map (containing commissioning histograms) using the
      contents of the root file (accessible using
      SiStripCommissioningFile). Converts from a map using "directory
      string" as its key to a map that instead uses the "FEC key". */
  void fillHistoMap();
  
  void fedCabling();
  void apvTiming();
  void fedTiming();
  void optoScan();
  void vpspScan();
  void pedestals();

 private: // ---------- member data ----------
  
  /** Client (Input) file */
  std::string filename_;
  
  sistrip::Task task_;
  sistrip::View view_;

  sistrip::SummaryHisto histo_;
  sistrip::SummaryType type_;
  
  /** Top-level directory for summary histogram. */
  std::string level_;

  /** */
  std::string path_;
  
  /** Granularity for summary histogram. */
  sistrip::Granularity gran_;

  /** Client (Input) file */
  SiStripCommissioningFile* file_;
  
  /** Run number */
  uint16_t run_;
  
  /** Map containing commissioning histograms. */
  HistosMap map_;
  
};

#endif // DQM_SiStripCommissioningSummary_SiStripOfflineClient_H
