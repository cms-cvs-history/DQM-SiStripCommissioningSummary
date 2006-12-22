#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
#include "DQM/SiStripCommissioningSummary/bin/stubs/ConfigParser.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
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
  
  // Iterate through tasks
  for ( uint32_t cntr = 0; cntr <= sistrip::invalid_; cntr++ ) {
    sistrip::Task in = static_cast<sistrip::Task>(cntr);
    string str = SiStripHistoNamingScheme::task(in);
    sistrip::Task out = SiStripHistoNamingScheme::task(str);
    if ( out != sistrip::UNKNOWN_TASK ) {
      const vector<ConfigParser::SummaryPlot>& summaries = cfg.summaryPlots(out);
      if ( !summaries.empty() ) {
	cout << "[test_ConfigParser] Retreiving SummaryPlots for task: " << str << endl;
	vector<ConfigParser::SummaryPlot>::const_iterator iter = summaries.begin();
	for ( ; iter != summaries.end(); iter++ ) { cout << (*iter) << endl; }
      }
    }
  }
  
  return 0;

}
