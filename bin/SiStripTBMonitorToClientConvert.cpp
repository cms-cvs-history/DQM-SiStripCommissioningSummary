#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBMonitorToClientConvert.h"
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

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;


/** 
    @author : M.Wingham
    @brief : Reads in Commissioning 
    TBMonitor-histogram file(s) and writes client TProfiles to file. 
*/

int main( int argc, char* argv[]) {

  // Default vaalues for arguments

  std::string cfg_file = "convert_cfg.dat";
  std::string tbmonitor_file = "";
  std::string client_file = "";
  sistrip::RunType run_type = sistrip::UNKNOWN_RUN_TYPE;

 // Read in arguments to main

  if ( argc > 1 ) { 
    cfg_file  = argv[1]; 
    cout << "Reading config. file: " << cfg_file << endl;
    ifstream in;
    in.open( cfg_file.c_str() );
    if( !in ) {
      stringstream ss;
      ss  << "[" << __PRETTY_FUNCTION__ << "]"
	  << " File could not be opened at '" 
	  << cfg_file << "'";
      cerr << ss.str() << endl;
    } else {
      while ( !in.eof() ) {
	string data;
	getline(in,data); 

	//ignore string after # symbol
	string::size_type pos = 0;
	while (pos != string::npos) {
	  pos = data.find("#",pos);
	  if (pos !=string::npos) {data.erase(pos,(data.size()-pos));}
	}

	//remove whitespace
	pos = 0;
	while (pos != string::npos) {
	  pos = data.find(" ",pos);
	  if (pos !=string::npos) {data.erase(pos,1);}
	}

	//interpret data here....

	pos = data.find("TBFile=",0);
	if (pos != string::npos) {
	  tbmonitor_file = data.substr(pos+7);}

	pos = data.find("ClientFile=",0);
	if (pos != string::npos) {
	  client_file = data.substr(pos+11);}

	pos = data.find("CommissioningTask=",0);
	if (pos != string::npos) {
	  string commissioningtask = data.substr(pos+18);
	  if (commissioningtask == sistrip::undefinedRunType_) {
	    run_type = sistrip::UNDEFINED_RUN_TYPE;}
	  else if (commissioningtask == sistrip::unknownRunType_) {
	    run_type = sistrip::UNKNOWN_RUN_TYPE;}
	  else if (commissioningtask == sistrip::fedCabling_) {
	    run_type = sistrip::FED_CABLING;}
	  else if (commissioningtask == sistrip::apvTiming_) {
	    run_type = sistrip::APV_TIMING;}
	  else if (commissioningtask == sistrip::fedTiming_) {
	    run_type = sistrip::FED_TIMING;}
	  else if (commissioningtask == sistrip::optoScan_) {
	    run_type = sistrip::OPTO_SCAN;}
	  else if (commissioningtask == sistrip::vpspScan_) {
	    run_type = sistrip::VPSP_SCAN;}
	  else if (commissioningtask == sistrip::pedestals_) {
	    run_type = sistrip::PEDESTALS;}
	  else if (commissioningtask == sistrip::apvLatency_) {
	    run_type = sistrip::APV_LATENCY;}}
      }
      in.close();
    }
  }

  if ( argc > 2 ) { tbmonitor_file  = argv[2]; }
  if ( argc > 3 ) { client_file =  argv[3]; }
  if ( argc > 4 ) { run_type  = static_cast<sistrip::RunType>( atoi( argv[4] ) ); }

  std::cout << " SiStripTBMonitorToClientConvert:" << std::endl
	    << " tbmonitor file:                 " << tbmonitor_file << std::endl
	    << " client file:                    " << client_file << std::endl
	    << " commissioning task:             " << SiStripEnumsAndStrings::runType( run_type ) << std::endl
	    << std::endl;

  // Convert
  SiStripTBMonitorToClientConvert tbmonitor( tbmonitor_file, client_file, run_type );
  if ( !tbmonitor.convert() ) {
    std::cout << "[main]: Error with TBMonitor conversions."
	      << " TBMonitor file is either not-present or"
	      << " contains no TH1F histograms." << std::endl;}

  //Message Loggger

  try { 

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

