#ifndef DQM_SiStripCommissioningSummary_SiStripTBMonitorToClientConvert_h
#define DQM_SiStripCommissioningSummary_SiStripTBMonitorToClientConvert_h

#include <map>
#include <string>
#include <vector>

//common
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"

//DQM/SiStripCommissioningSummary
#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"

//root
#include "TProfile.h"
#include "TDirectory.h"

/** 
    @file : DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBMonitorToClientConvert.h
    @class : SiStripTBMonitorToClientConvert
    @author : M.Wingham

    @brief : Reads in TBMonitor file(s), renames the TProfiles of interest (in accordance with the scheme in DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h) and writes them to a "client" file. 
*/

class SiStripTBMonitorToClientConvert {

 public:

  /** Constructor */
  explicit SiStripTBMonitorToClientConvert(std::string& client_path, std::string& tb_path, sistrip::Task = sistrip::PEDESTALS);

  /** Destructor */
  ~SiStripTBMonitorToClientConvert();

  /** Reads TBMonitor file(s), extracts TProfiles that match the commissioning task of interest, reformats them into "client" histograms (naming scheme as defined in DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h) and writes them to file.*/
  bool convert();  

  /** Sets run number based on file name(s) */
  void setRunInfo();

 private:
  
  /** Takes task name as the argument and updates the "task id" : a common sub-string of the names of histograms for storage.*/
  static std::string taskId(sistrip::Task);

  /** Unpacks TBMonitor histogram name. */
  SiStripHistoNamingScheme::HistoTitle histoTitle(const std::string&);
  
 /** Splits Pedestals Profile on the module-level into 2 or 3 Profiles on the LLD-channel-level.*/
  void lldPedestals(TProfile& module, std::vector<TProfile>& llds); 
  
  /** TBMonitor file, Client file. */
  std::string tb_path_, client_path_;

  /** Commissioning task. */
  sistrip::Task task_;

  /** Commissioning File View */
  sistrip::View view_;

  /** TBMonitor file.  Client file. */
  SiStripCommissioningFile* tb_, *client_;

  /** Task id found in TBMonitor histogram title */
  std::string taskId_;

  /** Run number */
  unsigned int run_;
 
};

#endif // DQM_SiStripCommissioningSummary_SiStripTBMonitorToClientConvert_h
