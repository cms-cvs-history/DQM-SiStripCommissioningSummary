#ifndef DQM_SiStripCommissioningSummary_SiStripOfflineClient_H
#define DQM_SiStripCommissioningSummary_SiStripOfflineClient_H

#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"
#include "DQM/SiStripCommissioningSummary/bin/stubs/ConfigParser.h"
#include <boost/cstdint.hpp>
#include <vector>
#include <string>
#include <map>

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
  
  SiStripOfflineClient( const std::string& root_file,
			const std::string& xml_file );
  ~SiStripOfflineClient();
  
  /** Performs analysis on each commissioning histogram found in the
      input root file and saves monitorables in map/histo. */
  void analysis();
  
 private: // ---------- private methods ----------
  
  /** Sets run number based on name of input root file. */
  void setRunNumber() {;} //@@ TO BE IMPLEMENTED
  
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

  /** Input .root file. */
  std::string rootFile_;

  /** Input .xml file. */
  std::string xmlFile_;

  /** Client input file. */
  SiStripCommissioningFile* file_;
  
  /** Commissioning runType. */
  sistrip::RunType runType_;

  /** Logical view. */
  sistrip::View view_;
    
  /** Run number. */
  uint16_t run_;
  
  /** Map containing commissioning histograms. */
  HistosMap map_;
  
  /** SummaryPlot objects. */
  std::vector<ConfigParser::SummaryPlot> plots_;
  
};

#endif // DQM_SiStripCommissioningSummary_SiStripOfflineClient_H
