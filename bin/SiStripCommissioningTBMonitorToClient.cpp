
#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningTBMonitorToClient.h"

#include <iostream>

//Message Logger
#include <boost/shared_ptr.hpp>
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"

using namespace std;

/** 
    @author : M.Wingham
    @brief : Reads in Commissioning 
    TBMonitor-histogram file(s) and writes client TProfiles to file. 
*/

int main(void) {

  //message logger info to standard out.
  edm::AssertHandler ah;
  boost::shared_ptr<edm::Presence> theMessageServicePresence = boost::shared_ptr<edm::Presence>(edm::PresenceFactory::get()->makePresence("MessageServicePresence").release());
  
  //configure
  string tb_path = "/afs/cern.ch/user/p/pwing/public/TBHistos/TBMonitor0030345_000.root";
  string client_path = "Client.root";
  sistrip::Task task = sistrip::OPTO_SCAN;

  //run
  SiStripCommissioningTBMonitorToClient tbToClient(tb_path,client_path,task);
  if(!tbToClient.convert()) {cout << "[main]: Error with TBMonitor conversions. TBMonitor file is either not-present or contains no TH1F histograms. " << endl;}
  return 0;
}
