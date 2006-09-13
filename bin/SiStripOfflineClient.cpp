#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
//#include "DQM/SiStripCommissioningSummary/bin/stubs/MessageLoggerInstance.h"
#include <iostream>
#include <string>

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
    @author : M.Wingham, R.Bainbridge
    @brief : Reads in client root file(s), performs analysis and
    creates summary histograms.
*/
int main( int argc, char* argv[] ) {

  // Default values for arguments
  std::string cfg_file = "cfg.dat";
  std::string file = "client.root";
  sistrip::SummaryHisto histo = sistrip::UNKNOWN_SUMMARY_HISTO;
  sistrip::SummaryType type = sistrip::UNKNOWN_SUMMARY_TYPE;
  sistrip::Granularity gran = sistrip::UNKNOWN_GRANULARITY;
  std::string level  = "DQMData/SiStrip/";
  
  // Read from file
  // BLAH
  
  // Read in args to main
  if ( argc > 1 ) { cfg_file  = argv[1]; }
  if ( argc > 2 ) { file  = argv[2]; }
  if ( argc > 3 ) { histo = static_cast<sistrip::SummaryHisto>( atoi( argv[3] ) ); }
  if ( argc > 4 ) { type  = static_cast<sistrip::SummaryType>( atoi( argv[4] ) ); }
  if ( argc > 5 ) { gran  = static_cast<sistrip::Granularity>( atoi( argv[5] ) ); }
  if ( argc > 6 ) { level = argv[6]; }
  
  std::cout << "OfflineClient:" << std::endl
	    << " file:        " << file << std::endl
 	    << " histo:       " << SiStripHistoNamingScheme::summaryHisto( histo ) << std::endl
 	    << " type:        " << SiStripHistoNamingScheme::summaryType( type ) << std::endl
	    << " granularity: " << SiStripHistoNamingScheme::granularity( gran ) << std::endl
	    << " top-level:   " << level << std::endl
	    << std::endl;

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
    SiStripOfflineClient client( file, histo, type, level, gran );
    
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
    cout << "std::exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "Unknown exception caught";
  }
  
  return 0;

}
