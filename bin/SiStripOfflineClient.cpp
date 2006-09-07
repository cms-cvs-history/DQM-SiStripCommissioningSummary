#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
#include "DQM/SiStripCommissioningSummary/bin/stubs/MessageLoggerInstance.h"
#include <iostream>
#include <string>

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/** 
    @author : M.Wingham, R.Bainbridge
    @brief : Reads in client root file(s), performs analysis and
    creates summary histograms.
*/
int main( int argc, char* argv[] ) {

  // Default values for arguments
  std::string file = "client.root";
  sistrip::SummaryHisto histo = sistrip::APV_TIMING_DELAY;
  sistrip::SummaryType type = sistrip::SUMMARY_DISTR;
  sistrip::Granularity gran = sistrip::MODULE;
  std::string level  = "DQMData/SiStrip/";
  
  // Read in args to main
  if ( argc > 1 ) { file  = argv[1]; }
  if ( argc > 2 ) { histo = static_cast<sistrip::SummaryHisto>( atoi( argv[2] ) ); }
  if ( argc > 3 ) { type  = static_cast<sistrip::SummaryType>( atoi( argv[3] ) ); }
  if ( argc > 4 ) { gran  = static_cast<sistrip::Granularity>( atoi( argv[4] ) ); }
  if ( argc > 5 ) { level = argv[5]; }
  
  std::cout << "OfflineClient:" << std::endl
	    << " file:        " << file << std::endl
 	    << " histo:       " << SiStripHistoNamingScheme::summaryHisto( histo ) << std::endl
 	    << " type:        " << SiStripHistoNamingScheme::summaryType( type ) << std::endl
	    << " granularity: " << SiStripHistoNamingScheme::granularity( gran ) << std::endl
	    << " top-level:   " << level << std::endl
	    << std::endl;

  // Use MessageLogger service
  //std::auto_ptr<MessageLoggerInstance> ml( MessageLoggerInstance::instance() );

  
  // Run client
  SiStripOfflineClient client( file, histo, type, level, gran );




  try {

    // Instantiate the plug-in manager
    edm::AssertHandler ah;
    
    // Load the message service plug-in
    boost::shared_ptr<edm::Presence> ml_presence;
    ml_presence = boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->
						    makePresence("MessageServicePresence").release() );
    
    // Construct path to "MessageLogger.cfg" file
    stringstream filename; 
    if ( getenv("CMSSW_BASE") != NULL ) { 
      filename << getenv("CMSSW_BASE") << "/src/DQM/SiStripCommon/data/"; 
    } else { 
      filename << "./"; 
    }
    filename << "MessageLogger.cfg";
    
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
    
  }
  
  // Catch any exceptions that may have been thrown
  catch ( cms::Exception& e ) {
    cout << "cms::Exception caught\n"
	 << e.explainSelf();
  }
  catch ( seal::Error& e ) {
    cout << "Exception caught\n"
	 << e.explainSelf();
  }
  catch ( exception& e ) {
    cout << "std::exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "Unknown exception caught";
  }

  edm::LogInfo("Test") << "INFO TEST" << std::endl; 
  LogDebug("Test") << "DEBUG TEST" << std::endl; 
  edm::LogError("Test") << "ERROR TEST" << std::endl; 
  
  return 0;
}
