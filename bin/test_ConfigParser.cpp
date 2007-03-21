#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
#include "DQM/SiStripCommissioningSummary/bin/stubs/ConfigParser.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "FWCore/PluginManager/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/** 
    @author R.Bainbridge
    @brief Tests summary histogram "xml file"
*/
int main( int argc, char* argv[] ) {

  // Read xml file
  std::string file = "test.xml";
  if ( argc > 1 ) { 
    file = argv[1]; 
    cout << "[test_ConfigParser]"
	 << " Summary histogram xml file: " 
	 << file << endl;
  } else {
    cout << "[test_ConfigParser]"
	 << " Using default value for summary histogram xml file: "
	 << file << endl;
  }
  
  // Parse xml file
  ConfigParser cfg;
  cfg.parseXML(file);
  
  // Iterate through RunTypes
  for ( uint32_t cntr = 0; cntr <= sistrip::invalid_; cntr++ ) {
    sistrip::RunType in = static_cast<sistrip::RunType>(cntr);
    string str = SiStripEnumsAndStrings::runType(in);
    sistrip::RunType out = SiStripEnumsAndStrings::runType(str);
    if ( out != sistrip::UNKNOWN_RUN_TYPE ) {
      const vector<ConfigParser::SummaryPlot>& summaries = cfg.summaryPlots(out);
      if ( !summaries.empty() ) {
	cout << "[test_ConfigParser] Retreiving SummaryPlots for run_type: " << str << endl;
	vector<ConfigParser::SummaryPlot>::const_iterator iter = summaries.begin();
	for ( ; iter != summaries.end(); iter++ ) { cout << (*iter) << endl; }
      }
    }
  }
  
  return 0;

}
