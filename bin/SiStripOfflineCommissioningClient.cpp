#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

/** 
    @author : M.Wingham
    @brief : Reads in Client histogram file(s), performs analysis
    and sumarises. Summary histograms are written to a separate file. 
*/

int main( int argc, char* argv[] ) {
  
  // Default values for arguments
  std::string file = "client.root";
  sistrip::SummaryHisto histo = sistrip::APV_TIMING_DELAY;
  sistrip::SummaryType type = sistrip::SUMMARY_SIMPLE_DISTR;
  std::string level  = "DQMData/SiStrip/ControlView/";

  // Read in args to main
  if ( argc > 1 ) { file  = argv[1]; }
  if ( argc > 2 ) { histo = atoi( argv[2] ); }
  if ( argc > 3 ) { type  = atoi( argv[3] ); }
  if ( argc > 4 ) { level = argv[4]; }
  
  std::cout << "OfflineClient:" << std::endl
	    << " file:  " << file << std::endl
 	    << " histo: " << SiStripHistoNamingScheme::summaryHisto( histo ) << std::endl
 	    << " type:  " << SiStripHistoNamingScheme::summaryType( type ) << std::endl
	    << " level: " << level << std::endl;
  
  // Service allows use of MessageLogger
  edm::AssertHandler ah;
  boost::shared_ptr<edm::Presence> message = boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->makePresence("MessageServicePresence").release() );
  
  // Run client
  SiStripOfflineCommissioningClient client( file, histo, type, level );
  
  return 0;
}
