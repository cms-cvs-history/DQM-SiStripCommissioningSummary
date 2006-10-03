
#ifndef DQM_SiStripCommissioningSummary_SiStripTBHtmlSummary_h
#define DQM_SiStripCommissioningSummary_SiStripTBHtmlSummary_h

#include <boost/cstdint.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <utility>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

/** 
    @file : DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBHtmlSummary.h
    @class : SiStripTBHtmlSummary.h
    @author : M.Wingham

    @brief :  Reads in commissioning html file and summarises requested
    monitorable. For OptoScan file the monitorable with for the gain closest
    to the specified target is recorded.
*/

class SiStripTBHtmlSummary {

 public: 

  /** Constructor */
  SiStripTBHtmlSummary(std::string htmlFile, std::string summaryPath, std::string monitorable, std::string summaryType, float targetGain = 0.8, uint16_t lldChan = 0, uint16_t tick = 0);

  /** Destructor */
  ~SiStripTBHtmlSummary();

  /** Html file parse method.*/
  bool parseHtml();

  /** Summarises stored monitorables in a TH2F histogram */
  bool summary2D();

  /** Writes the histogram to the desired file. */
  bool write();

 private:

  /** */
  bool openFile();
  std::string summaryFile();
  std::string summaryDir();
  std::string summaryName();
  void findGain();

  std::string htmlFile_;
  std::string summaryPath_;
  std::string monitorable_;
  std::string summaryType_;
  float targetGain_;
  uint16_t lldChan_;
  uint16_t tick_;
  std::map< std::string, std::vector< std::pair<float, float> > > dataMap_;
  TFile* file_;
  TH1* histo_;

  uint16_t count_;
  uint16_t apvcount_;
  uint16_t gaincount_;
  uint16_t tickcount_;

};

#endif //DQM_SiStripCommissioningSummary_SiStripTBHtmlSummary_h
