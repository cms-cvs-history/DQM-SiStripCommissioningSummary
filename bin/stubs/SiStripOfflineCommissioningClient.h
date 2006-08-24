#ifndef DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H
#define DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H

#include "DQM/SiStripCommissioningAnalysis/interface/CommissioningAnalysis.h" 
/* #include "DQM/SiStripCommissioningSummary/interface/CommissioningSummary.h" */
#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"
#include <boost/cstdint.hpp>
#include "TProfile.h"
#include "TFile.h"
#include <string>

/**
   @file : DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h
   @class : SiStripOfflineCommissioningClient
   @author: M.Wingham
   
   @brief : Class which reads TProfile commissioning histograms from a "client" file, performs an analysis to extract "commissioning monitorables" for each device and adds them to a summary map. The contents of the map (values and errors) are then histogrammed and written to a separate file. 
*/
class SiStripOfflineCommissioningClient {
  
 public:

  typedef std::vector<TProfile*> Histos;
  typedef std::map< uint32_t, Histos > HistosMap;
  
  SiStripOfflineCommissioningClient( std::string filename,
				     sistrip::SummaryHisto,
				     sistrip::SummaryType,
				     std::string level );
  ~SiStripOfflineCommissioningClient();
  
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
  void fillMap();
  
  void apvTiming();
  void fedTiming();
  void pedestals();
  void optoScan();
  void vpspScan();

 private: // ---------- member data ----------
  
  /** Client (Input) file */
  std::string filename_;
  
  sistrip::Task task_;
  sistrip::View view_;

  sistrip::SummaryHisto histo_;
  sistrip::SummaryType type_;
  
  /** Directory level of summary histogram. */
  std::string level_;

  /** Client (Input) file */
  SiStripCommissioningFile* file_;
  
  /** Run number */
  uint16_t run_;
  
  /** Map containing commissioning histograms. */
  HistosMap map_;
  
};

#endif // DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H
