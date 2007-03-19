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
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/** 
    @author : M.Wingham, R.Bainbridge
    @brief : Reads in client root file(s), performs analysis and
    creates summary histograms.
*/
int main( int argc, char* argv[] ) {

  using namespace std;
  using namespace sistrip;

  try { 

    // -------------------- Instantiating MessageLogger service --------------------

    string cfgFile_("DQM/SiStripCommon/data/MessageLogger.cfg");

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
      ss << "[SiStripOfflineClient.cpp]"
	 << " File could not be opened at '" 
	 << filename.str() << "'";
      cerr << ss.str() << endl;
    } else {
      //in >> config;
      while ( !in.eof() ) {
	string data;
	getline(in,data); 
	config << data << "\n";
      }
      in.close();
      cout << "[SiStripOfflineClient.cpp]"
	   << " Message Logger configuration read from '" 
	   << filename.str() << "' file: " << endl
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
    
    // Read in histogram root and xml config files
    std::string root_file = "source.root";
    std::string xml_file = "summary.xml";
    if ( argc == 2 ) { 
      if ( string(argv[1]).find(".root") == string::npos ) { 
	edm::LogError(mlDqmClient_)
	  << "[SiStripOfflineClient.cpp]"
	  << " Unexpected filename!"
	  << " Filename should have extension \".root\"."
	  << " Usage: SiStripOfflineClient <.root file> <.xml file>";
	return 1;
      }
      root_file = argv[1]; 
      cout << "Reading root file: " << root_file << endl;
    } else if ( argc == 3 ) { 
      if ( string(argv[1]).find(".root") == string::npos ) { 
	edm::LogError(mlDqmClient_)
	  << " Unexpected filename!"
	  << " Filename should have extension \".root\"."
	  << " Usage: SiStripOfflineClient <.root file> <.xml file>";
	return 1;
      }
      root_file = argv[1]; 
      cout << "Reading root file: " << root_file << endl;
      if ( string(argv[2]).find(".xml") == string::npos ) { 
	edm::LogError(mlDqmClient_)
	  << "[SiStripOfflineClient.cpp]"
	  << " Unexpected filename!"
	  << " Filename should have extension \".xml\"."
	  << " Usage: SiStripOfflineClient <.root file> <.xml file>";
	return 1; 
      }
      xml_file = argv[2]; 
      cout << "Reading xml file: " << xml_file << endl;
    } else {
      edm::LogError(mlDqmClient_)
	<< "[SiStripOfflineClient.cpp]"
	<< " Usage: SiStripOfflineClient <.root file> <.xml file>";
      return 1;
    }
    
    // Create offline client
    SiStripOfflineClient client( root_file, xml_file );
    
  }
  catch ( cms::Exception& e ) {
    cout << "[SiStripOfflineClient.cpp]"
	 << " cms::Exception caught\n"
	 << e.explainSelf();
  }
  catch ( seal::Error& e ) {
    cout << "[SiStripOfflineClient.cpp]"
	 << " seal::Error caught\n"
	 << e.explainSelf();
  }
  catch ( exception& e ) {
    cout << "[SiStripOfflineClient.cpp]"
	 << " std::exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "[SiStripOfflineClient.cpp]"
	 << " Unknown exception caught";
  }
  
  return 0;

}
