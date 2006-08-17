#include "DQM/SiStripCommissioningSummary/bin/stubs/ConvertTBMonitorHistos.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"
#include <boost/shared_ptr.hpp>
#include <iostream>

/** 
    @author : M.Wingham
    @brief : Reads in Commissioning 
    TBMonitor-histogram file(s) and writes client TProfiles to file. 
*/

int main() {
  
  // Service allows use of MessageLogger
  edm::AssertHandler ah;
  boost::shared_ptr<edm::Presence> message = 
    boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->makePresence("MessageServicePresence").release() );
  
  // Some configuration
  std::string tbmonitor_file = "/afs/cern.ch/user/p/pwing/public/TBHistos/TBMonitor0030349_000.root";
  std::string client_file = "client.root";
  sistrip::Task task = sistrip::PEDESTALS;
  
  // Run client
  ConvertTBMonitorHistos tbmonitor( tbmonitor_file, client_file, task );
  if ( !tbmonitor.convert() ) {
    std::cout << "[main]: Error with TBMonitor conversions."
	      << " TBMonitor file is either not-present or"
	      << " contains no TH1F histograms." << std::endl;
  }
  
  return 0;
}

