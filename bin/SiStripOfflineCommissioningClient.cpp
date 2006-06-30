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

//-----------------------------------------------------------------------------

void messageService(void) {

  edm::AssertHandler ah;
  boost::shared_ptr<edm::Presence> theMessageServicePresence = boost::shared_ptr<edm::Presence>(edm::PresenceFactory::get()->makePresence("MessageServicePresence").release());}

//-----------------------------------------------------------------------------

int main(void) {

  messageService();

  //define arguments
  string client_path = "Client_0030349.root";
  string summary_path = "Summary";
  string summaryLevel = "ControlView/";

  SiStripOfflineCommissioningClient offlineClient(client_path, summary_path, summaryLevel);
  offlineClient.analysis();
  return 0;
}

