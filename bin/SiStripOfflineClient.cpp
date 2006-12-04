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
    @author : M.Wingham, R.Bainbridge
    @brief : Reads in client root file(s), performs analysis and
    creates summary histograms.
*/
int main( int argc, char* argv[] ) {

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
      ss << "[SiStripOfflineClient]"
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
      cout << "[SiStripOfflineClient]"
	   << " Message Logger configuration read from '" 
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
   
    // -------------------- Run offline client --------------------
    
    // Read in client (root) file xml config file
    std::string root_file = "client.root";
    std::string xml_file = "summary.xml";
    if ( argc > 1 ) { 
      root_file = argv[1]; 
      cout << "Reading root file: " << root_file << endl;
    } else if ( argc > 2 ) { 
      root_file = argv[1]; 
      cout << "Reading root file: " << root_file << endl;
      xml_file = argv[2]; 
      cout << "Reading xml file: " << xml_file << endl;
    } else {
      cout << "Reading \"client.root\" and \"summary.xml\" files!" << endl;
    }     
    
    // Create offline client
    SiStripOfflineClient client( root_file, xml_file );
    
  }
  catch ( cms::Exception& e ) {
    cout << "[SiStripOfflineClient]"
	 << " cms::Exception caught\n"
	 << e.explainSelf();
  }
  catch ( seal::Error& e ) {
    cout << "[SiStripOfflineClient]"
	 << " seal::Error caught\n"
	 << e.explainSelf();
  }
  catch ( exception& e ) {
    cout << "[SiStripOfflineClient]"
	 << " std::exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "[SiStripOfflineClient]"
	 << " Unknown exception caught";
  }
  
  return 0;

}
