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
  std::string file  = "client.root";
  std::string level = "ControlView/";
  std::string task  = "APV_TIMING";
  std::string view  = "CONTROL";

  // Read in args to main
  if ( argc > 1 ) { file = argv[1]; }
  if ( argc > 2 ) { file = argv[2]; }
  if ( argc > 3 ) { file = argv[3]; }
  if ( argc > 4 ) { file = argv[4]; }

  std::cout << "OfflineClient:"
	    << " file:  " << file << std::endl
	    << " level: " << level << std::endl
	    << " task:  " << task << std::endl
	    << " view:  " << view << std::endl;
  
  // Service allows use of MessageLogger
  edm::AssertHandler ah;
  boost::shared_ptr<edm::Presence> message = boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->makePresence("MessageServicePresence").release() );
  
  // Run client
  SiStripOfflineCommissioningClient client( file, level );
  client.analysis();
  
  return 0;
}

