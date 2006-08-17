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

  typedef std::vector<const TProfile*> Histos;
  typedef std::map< uint16_t, Histos > HistosMap;
  
  SiStripOfflineCommissioningClient( std::string file, 
				     std::string level = "ControlView/" );
  ~SiStripOfflineCommissioningClient();
  
  /** Performs analysis on each commissioning histogram found in the
      input root file and saves monitorables in map/histo. */
  void analysis();
  
 private: // ---------- private methods ----------
  
  /** Sets run number based on name of input root file. */
  void setRunNumber();
  
  /** Constructs summary histogram according to commissioning task. */
  void createSummaryHisto();
  
  /** Fills map (containing commissioning histograms) using the contents
      of the root file (accessible using SiStripCommissioningFile). */
  void fillMap();
  
  /** Converts from a map using "directory string" as its key to a map
      that instead uses the "FEC key". */
  void convertMap( std::map< std::string, std::vector<TProfile*> >& );
  
  /** Fills the summary histogram and writes it to file. */
  void writeSummaryHistoToFile();

  void apvTiming();

 private: // ---------- member data ----------
  
  /** Client (Input) file */
  std::string file_;
  
  /** Output file name */
  std::string summary_path_;

  /** Commissioning task */
  sistrip::Task task_;

  /** Commissioning File View */
  sistrip::View view_;

  /** Summary */
  std::string level_;
  /*   CommissioningSummary* c_summary_; */
  /*   CommissioningSummary* c_summary2_; */

  /** Summary file name */
  SiStripCommissioningFile* summary_;
  
  /** Client (Input) file */
  SiStripCommissioningFile* client_;
  
  /** Run number */
  uint16_t run_;
  
  /** Map containing commissioning histograms. */
  HistosMap map_;
  
};

#endif // DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H
