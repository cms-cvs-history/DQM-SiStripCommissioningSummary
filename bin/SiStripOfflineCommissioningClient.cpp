#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h"
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"

//Message Logger
#include <boost/shared_ptr.hpp>
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/PresenceFactory.h"
#include "FWCore/Utilities/interface/ProblemTracker.h"

using namespace std;

/** 
    @author : M.Wingham

    @brief : Reads in Client histogram file(s), performs analysis
and sumarises. Summary histograms are written to a separate file. 
*/

int main(void) {

  //message logger info to standard out.
   edm::AssertHandler ah;
   boost::shared_ptr<edm::Presence> theMessageServicePresence = boost::shared_ptr<edm::Presence>(edm::PresenceFactory::get()->makePresence("MessageServicePresence").release());
  
  //configure
  string client_path = "Client_0030349.root";
  string summary_path = "Summary";
  string summaryLevel = "ControlView/FecCrate0/FecSlot2/FecRing7/CcuAddr123/";

  //run
  SiStripOfflineCommissioningClient offlineClient(client_path, summary_path, summaryLevel);
  offlineClient.analysis();
  return 0;
}

