#ifndef DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H
#define DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H

//analysis
#include "DQM/SiStripCommissioningAnalysis/interface/CommissioningAnalysis.h"
//summary
#include "DQM/SiStripCommissioningSummary/interface/SiStripSummary.h"
#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"
//root
#include "TFile.h"
#include "TProfile.h"

#include <string>

/**
   @file : DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h
   @class : SiStripOfflineCommissioningClient
   @author: M.Wingham

   @brief : Class which reads TProfile commissioning histograms from a "client" file, performs an analysis to extract "commissioning monitorables" for each device and adds them to a summary map. The contents of the map (values and errors) are then histogrammed and written to a separate file.
*/

class SiStripOfflineCommissioningClient {
  
 public:
  
  /** Constructor */
  SiStripOfflineCommissioningClient(string client_file, string summary_file = "Summary.root", string summaryLevel = "ControlView/");

  /** Destructor */
  ~SiStripOfflineCommissioningClient();
  
  /** Performs the analysis of each histogram in the file and saves the monitorables in the summary map. */
  void analysis();

 private:

  /** Sets run number based of Client file name.*/
  void setRunInfo();

  /** Constructs and names the summary class according to commissioning task.*/
  void prepareSummary();

  /** Fills private data member commissioning_map_ using the contents of SiStripCommissioningFile client_*/
  void fillCommissioningMap();

  /** Converts map indexed by directory string to "commissioning map" (indexed by fec-key) */
  void convertMap(map< string, vector<TProfile*> >*);

  /** Fills the summary histogram and writes it to file. */
  void writeSummary();

  /** Client (Input) file */
  string client_path_;

  /** Output file name */
  string summary_path_;

  /** Commissioning task */
  sistrip::Task task_;

  /** Commissioning File View */
  sistrip::View view_;

  /** Summary */
  string dirLevel_;
  SiStripSummary* c_summary_;
  SiStripSummary* c_summary2_;

  /** Summary file name */
  SiStripCommissioningFile* summary_;
  
  /** Client (Input) file */
  SiStripCommissioningFile* client_;
  
  /** Target gain for bias-gain task */
  double targetGain_;

  /** Run number */
  unsigned int run_;

  /** Commissioning map */
  map< unsigned int,vector<const TProfile*> >* commissioning_map_;
};

#endif // DQM_SiStripCommissioningSummary_SiStripOfflineCommissioningClient_H
