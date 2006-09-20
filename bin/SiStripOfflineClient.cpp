#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
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

  std::string cfg_file = "client_cfg.dat";
  std::string file = "client.root";
  std::string level  = "ControlView";
  sistrip::SummaryHisto histo = sistrip::UNKNOWN_SUMMARY_HISTO;
  sistrip::SummaryType type = sistrip::UNKNOWN_SUMMARY_TYPE;
  sistrip::Granularity gran = sistrip::UNKNOWN_GRAN;
  
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

	//interpret data here....

	string::size_type pos = data.find("ClientFile=",0);
	if (pos != string::npos) {
	  file = data.substr(pos+11);}

	pos = data.find("SummaryLevel=",0);
	if (pos != string::npos) {
	  level = data.substr(pos+13);}
	
	pos = data.find("SummaryHisto=",0);
	if (pos != string::npos) {
	  string summaryhisto = data.substr(pos+13);
	  if (summaryhisto == sistrip::undefinedSummaryHisto_) {
	    histo = sistrip::UNDEFINED_SUMMARY_HISTO;}
	  else if (summaryhisto == sistrip::undefinedSummaryHisto_) {
	    histo = sistrip::UNDEFINED_SUMMARY_HISTO;}
	  else if (summaryhisto == sistrip::apvTimingTime_) {
	    histo = sistrip::APV_TIMING_TIME;}
	  else if (summaryhisto == sistrip::apvTimingMax_) {
	    histo = sistrip::APV_TIMING_MAX_TIME;}
	  else if (summaryhisto == sistrip::apvTimingDelay_) {
	    histo = sistrip::APV_TIMING_DELAY;}
	  else if (summaryhisto == sistrip::apvTimingError_) {
	    histo = sistrip::APV_TIMING_ERROR;}
	  else if (summaryhisto == sistrip::apvTimingBase_) {
	    histo = sistrip::APV_TIMING_BASE;}
	  else if (summaryhisto == sistrip::apvTimingPeak_) {
	    histo = sistrip::APV_TIMING_PEAK;}
	  else if (summaryhisto == sistrip::apvTimingHeight_) {
	    histo = sistrip::APV_TIMING_HEIGHT;}
	  else if (summaryhisto == sistrip::fedTimingTime_) {
	    histo = sistrip::FED_TIMING_TIME;}
	  else if (summaryhisto == sistrip::fedTimingMax_) {
	    histo = sistrip::FED_TIMING_MAX_TIME;}
	  else if (summaryhisto == sistrip::fedTimingDelay_) {
	    histo = sistrip::FED_TIMING_DELAY;}
	  else if (summaryhisto == sistrip::fedTimingError_) {
	    histo = sistrip::FED_TIMING_ERROR;}
	  else if (summaryhisto == sistrip::fedTimingBase_) {
	    histo = sistrip::FED_TIMING_BASE;}
	  else if (summaryhisto == sistrip::fedTimingPeak_) {
	    histo = sistrip::FED_TIMING_PEAK;}
	  else if (summaryhisto == sistrip::fedTimingHeight_) {
	    histo = sistrip::FED_TIMING_HEIGHT;}
	  else if (summaryhisto == sistrip::optoScanLldBias_) {
	    histo = sistrip::OPTO_SCAN_LLD_BIAS_SETTING;}
	  else if (summaryhisto == sistrip::optoScanLldGain_) {
	    histo = sistrip::OPTO_SCAN_LLD_GAIN_SETTING;}
	  else if (summaryhisto == sistrip::optoScanMeasGain_) {
	    histo = sistrip::OPTO_SCAN_MEASURED_GAIN;}
	  else if (summaryhisto == sistrip::optoScanZeroLight_) {
	    histo = sistrip::OPTO_SCAN_ZERO_LIGHT_LEVEL;}
	  else if (summaryhisto == sistrip::optoScanLinkNoise_) {
	    histo = sistrip::OPTO_SCAN_LINK_NOISE;}
	  else if (summaryhisto == sistrip::optoScanBaseLiftOff_) {
	    histo = sistrip::OPTO_SCAN_BASELINE_LIFT_OFF;}
	  else if (summaryhisto == sistrip::optoScanLaserThresh_) {
	    histo = sistrip::OPTO_SCAN_LASER_THRESHOLD;}
	  else if (summaryhisto == sistrip::optoScanTickHeight_) {
	    histo = sistrip::OPTO_SCAN_TICK_HEIGHT;}
	  else if (summaryhisto == sistrip::vpspScanBothApvs_) {
	    histo = sistrip::VPSP_SCAN_BOTH_APVS;}
	  else if (summaryhisto == sistrip::vpspScanApv0_) {
	    histo = sistrip::VPSP_SCAN_APV0;}
	  else if (summaryhisto == sistrip::vpspScanApv1_) {
	    histo = sistrip::VPSP_SCAN_APV1;}
	  else if (summaryhisto == sistrip::pedestalsMean_) {
	    histo = sistrip::PEDESTALS_MEAN;}
	  else if (summaryhisto == sistrip::pedestalsSpread_) {
	    histo = sistrip::PEDESTALS_SPREAD;}
	  else if (summaryhisto == sistrip::pedestalsMax_) {
	    histo = sistrip::PEDESTALS_MAX;}
	  else if (summaryhisto == sistrip::pedestalsMin_) {
	    histo = sistrip::PEDESTALS_MIN;}
	  else if (summaryhisto == sistrip::noiseMean_) {
	    histo = sistrip::NOISE_MEAN;}
	  else if (summaryhisto == sistrip::noiseSpread_) {
	    histo = sistrip::NOISE_SPREAD;}
	  else if (summaryhisto == sistrip::noiseMax_) {
	    histo = sistrip::NOISE_MAX;}
	  else if (summaryhisto == sistrip::noiseMin_) {
	    histo = sistrip::NOISE_MIN;}
	  else if (summaryhisto == sistrip::numOfDead_) {
	    histo = sistrip::NUM_OF_DEAD;}
	  else if (summaryhisto == sistrip::numOfNoisy_) {
	    histo = sistrip::NUM_OF_NOISY;}}

	pos = data.find("SummaryType=",0);
	if (pos != string::npos) {
	  string summarytype = data.substr(pos+12);
	  if (summarytype == sistrip::undefinedSummaryType_) {
	    type = sistrip::UNDEFINED_SUMMARY_TYPE;}
	  else if (summarytype == sistrip::summaryDistr_) {
	    type = sistrip::SUMMARY_DISTR;}
	  else if (summarytype == sistrip::summary1D_) {
	    type = sistrip::SUMMARY_1D;}
	  else if (summarytype == sistrip::summary2D_) {
	    type = sistrip::SUMMARY_2D;}
	  else if (summarytype == sistrip::summaryProf_) {
	    type = sistrip::SUMMARY_PROF;}}

	pos = data.find("SummaryGran=",0);
	if (pos != string::npos) {
	  string granularity = data.substr(pos+12);
	  if (granularity == sistrip::tracker_) {
	    gran = sistrip::TRACKER;}
	  else if (granularity == sistrip::partition_) {
	    gran = sistrip::PARTITION;}
	  else if (granularity == sistrip::module_) {
	    gran = sistrip::CCU_CHAN;}
	  else if (granularity == sistrip::lldChan_) {
	    gran = sistrip::LLD_CHAN;}
	  else if (granularity == sistrip::apv_) {
	    gran = sistrip::APV;}}
      }
      in.close();
    }
  }

  if ( argc > 2 ) { file  = argv[2]; }
  if ( argc > 3 ) { histo = static_cast<sistrip::SummaryHisto>( atoi( argv[3] ) ); }
  if ( argc > 4 ) { type  = static_cast<sistrip::SummaryType>( atoi( argv[4] ) ); }
  if ( argc > 5 ) { gran  = static_cast<sistrip::Granularity>( atoi( argv[5] ) ); }
  if ( argc > 6 ) { level = argv[6]; }
  
  cout << "SiStripOfflineClient:" << endl
	    << " file:        " << file << endl
 	    << " histo:       " << SiStripHistoNamingScheme::summaryHisto( histo ) << endl
 	    << " type:        " << SiStripHistoNamingScheme::summaryType( type ) << endl
	    << " granularity: " << SiStripHistoNamingScheme::granularity( gran ) << endl
	    << " top-level:   " << level << endl
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
    cout << "exception caught\n"
	 << e.what();
  }
  catch (...) {
    cout << "Unknown exception caught";
  }
  
  return 0;

}
