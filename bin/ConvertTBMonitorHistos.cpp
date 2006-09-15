#include "DQM/SiStripCommissioningSummary/bin/stubs/ConvertTBMonitorHistos.h"
#include <iostream>

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
#include <string>
#include <vector>

using namespace std;

/** 
    @author : M.Wingham
    @brief : Reads in Commissioning 
    TBMonitor-histogram file(s) and writes client TProfiles to file. 
*/

int main( int argc, char* argv[] ) {
  
  // Default values for arguments
  std::string input_file = "TBMonitor.root";
  sistrip::Task task = sistrip::UNKNOWN_TASK;
  std::string output_file = "client.root";
  
  // Command line args
  if ( argc > 1 ) { input_file  = argv[1]; }
  if ( argc > 2 ) { task = static_cast<sistrip::Task>( atoi( argv[2] ) ); }
  if ( argc > 3 ) { output_file  = argv[3]; }

  cout << "ConvertTBMonitorHistos:" << endl
       << " input file:  " << input_file << endl
       << " task:        " << SiStripHistoNamingScheme::task( task ) << endl
       << " output file: " << input_file << endl
       << endl;

  try { 

    string cfgFile_("DQM/SiStripCommon/data/MessageLoggerInstance.cfg");

    // Instantiate the plug-in manager
    edm::AssertHandler ah;
  
    // Load the message service plug-in
    boost::shared_ptr<edm::Presence> ml_presence;
    ml_presence = boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->
						    makePresence("MessageServicePresence").release() );
    
    // Construct path to "MessageLogger.cfg" file
    stringstream filename; 
    if ( getenv("CMSSW_BASE") != NULL ) { 
      filename << getenv("CMSSW_BASE") << "/src/"; 
    }
    filename << cfgFile_;
    
    // Read a configuration from the "MessageLogger.cfg" file
    stringstream config;
    ifstream in;
    in.open( filename.str().c_str() );
    if( !in ) {
      stringstream ss;
      ss  << "[" << __PRETTY_FUNCTION__ << "]"
	  << " File could not be opened at '" 
	  << filename.str() << "'";
      cerr << ss.str() << endl;
      throw cms::Exception("FileNotFound") << ss.str();
    } else {
      //in >> config;
      while ( !in.eof() ) {
	string data;
	getline(in,data); 
	config << data << "\n";
      }
      in.close();
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Configuration read from '" 
	   << filename.str() << "' file is: " << endl
	   << config.str() << endl;
    }
    
    // Establish the configuration
    boost::shared_ptr<vector<edm::ParameterSet> > pServiceSets;
    boost::shared_ptr<edm::ParameterSet> params_;
    edm::makeParameterSets( config.str(), params_, pServiceSets );

    // Create the services
    edm::ServiceToken tempToken( edm::ServiceRegistry::createSet(*pServiceSets.get()) );
    
    // Make the services available
    edm::ServiceRegistry::Operate operate(tempToken);
    
    // Run client
    ConvertTBMonitorHistos tbmonitor( input_file, output_file, task );
    if ( !tbmonitor.convert() ) {
      cout << "[main]: Error with TBMonitor conversions."
	   << " TBMonitor file is either not-present or"
	   << " contains no TH1F histograms." << endl;
    }
    
  }
  catch ( cms::Exception& e ) {
    cout << "cms::Exception caught\n"
	 << e.explainSelf();
  }
  catch ( seal::Error& e ) {
    cout << "Exception caught\n"
	 << e.explainSelf();
  }
  catch ( exception& e ) {
    cout << "exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "Unknown exception caught";
  }
  
  return 0;

}

