#include "DQM/SiStripCommissioningSummary/bin/stubs/MessageLoggerInstance.h"
#include <iostream>
#include <iomanip>

using namespace std;

// -----------------------------------------------------------------------------
// 
MessageLoggerInstance* MessageLoggerInstance::instance_ = 0;

// -----------------------------------------------------------------------------
// Private constructor
MessageLoggerInstance::MessageLoggerInstance() 
  : cfgFile_("DQM/SiStripCommon/data/MessageLoggerInstance.cfg")
  
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  instance_ = this;
}

// -----------------------------------------------------------------------------
// Public destructor
MessageLoggerInstance::~MessageLoggerInstance() { 
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  instance_ = 0; 
}

// -----------------------------------------------------------------------------
// 
MessageLoggerInstance* MessageLoggerInstance::instance() {
  if( !instance_ ) { 
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Creating instance of MessageLoggerInstance..."
	 << endl;
    instance_ = new MessageLoggerInstance(); 
  } else {
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Instance of MessageLoggerInstance already exists at 0x"
	 << hex << setw(8) << setfill('0') << instance_ << dec 
	 << endl;
  }
  return instance_;
}

// -----------------------------------------------------------------------------
// 
void MessageLoggerInstance::deleteInstance() {
  if( instance_ ) { 
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Deleting instance of MessageLoggerInstance at 0x"
	 << hex << setw(8) << setfill('0') << instance_ << dec 
	 << endl;
    delete instance_;
  } else {
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " No instance of MessageLoggerInstance exists"
	 << endl;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "FWCore/PluginManager/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// 
void MessageLoggerInstance::doRealWork() {
  useMessageLogger();
}

// -----------------------------------------------------------------------------
// 
void MessageLoggerInstance::putCodeHere() {
  edm::LogInfo("Test") << "INFO TEST" << std::endl; 
  LogDebug("Test") << "DEBUG TEST" << std::endl; 
  edm::LogError("Test") << "ERROR TEST" << std::endl; 
}

// -----------------------------------------------------------------------------
// 
void MessageLoggerInstance::useMessageLogger() {

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

    putCodeHere();
    
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

}

