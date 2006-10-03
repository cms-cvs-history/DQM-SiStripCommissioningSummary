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

  // Define summary definition containers

  vector<string> client_files;
  vector<ConfigParser::SummaryInfo> info;
  ConfigParser::SummaryInfo summaryInfo;
  
  // Read in config file if present

  if ( argc > 1 ) { 
    std::string cfg_file = cfg_file  = argv[1]; 
    cout << "Reading config. file: " << cfg_file << endl;
    client_files.clear();info.clear();

    ConfigParser cfg_info;
    cfg_info.parseXML(cfg_file);

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
   
    //Run offline client
    cfg_info.getFileNames(client_files);
    for (vector<string>::const_iterator ifile = client_files.begin(); ifile != client_files.end(); ifile++) {
      info = cfg_info.getSummaryInfo(*ifile);
      for (vector<ConfigParser::SummaryInfo>::const_iterator iinfo = info.begin(); iinfo != info.end(); iinfo++) {
	
    cout << "SiStripOfflineClient:" << endl
	 << " file:        " << *ifile << endl
	 << " histo:       " << SiStripHistoNamingScheme::summaryHisto( iinfo->histogram ) << endl
	 << " type:        " << SiStripHistoNamingScheme::summaryType( iinfo->type ) << endl
	 << " granularity: " << SiStripHistoNamingScheme::granularity( iinfo->granularity ) << endl
	 << " top-level:   " << iinfo->level << endl
	 << endl;

    SiStripOfflineClient client( *ifile, iinfo->histogram, iinfo->type, iinfo->level, iinfo->granularity );
      }
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
  }

  else {cout << "Requires xml filename as argument" << endl;}

  return 0;
  
}
