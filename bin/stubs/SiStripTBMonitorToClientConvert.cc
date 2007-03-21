#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBMonitorToClientConvert.h"
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
//common
#include "DQM/SiStripCommon/interface/UpdateTProfile.h"
//data formats
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"

#include <iostream> 
#include <iomanip>
#include <memory>
#include <sstream>
#include <cstdlib>

using namespace std;

SiStripTBMonitorToClientConvert::SiStripTBMonitorToClientConvert(string& tb_path, string& client_path, sistrip::RunType run_type) : 

  //initialise private data members
  tb_path_(tb_path),
  client_path_(client_path),
  runType_(run_type),
  view_(sistrip::CONTROL),
  tb_(0),
  client_(0),
  runTypeId_(""),
  run_(0)

{ 
  //Check Commissioning RunType
  if (runType_ == sistrip::UNKNOWN_RUN_TYPE) cout << "Unknown commissioning run_type. Value used: " << run_type << "; values accepted: Pedestals, ApvTiming, FedTiming, OptoScan, VpspScan, ApvLatency." << endl;

  if (runType_ == sistrip::UNDEFINED_RUN_TYPE) cout << "Undefined commissioning run_type. Value used: " << run_type << "; values accepted: Pedestals, ApvTiming, FedTiming, OptoScan, VpspScan, ApvLatency." << endl;
  
  setRunInfo();
  runTypeId_ = runTypeId(runType_);
  
  //construct and name and format output file...
  string name = client_path_.substr( 0, client_path_.find(".root",0));
  stringstream ss; ss << name << "_" << setfill('0') << setw(7) << run_ << ".root";
  client_ = new SiStripCommissioningFile(ss.str().c_str(), "RECREATE");
  client_->setDQMFormat(runType_,view_);
  
  //open TBMonitor file
  tb_ = new SiStripCommissioningFile(tb_path_.c_str(), "READ");
}

//-----------------------------------------------------------------------------

SiStripTBMonitorToClientConvert::~SiStripTBMonitorToClientConvert() {
  if (client_) delete client_;
  if (tb_) delete tb_;
}

//-----------------------------------------------------------------------------

bool SiStripTBMonitorToClientConvert::convert() {

  //Define histogram containers
  map< string, vector<TH1*> > profile_map;
  map< unsigned int,vector<TProfile> > commissioning_map;
  
  //find TH1s from TBMonitor file
  if (!tb_) return false;
  tb_->findHistos(tb_->top(),&profile_map);
  if (profile_map.empty()) return false;
  
  //Loop all commissioning tprofiles. Fill map, indexing with CCU Address fec-key. Update client file directory structure.
  
  for (map< string, vector<TH1*> >::iterator ihistset = profile_map.begin(); ihistset != profile_map.end(); ihistset++) {
    for (vector<TH1*>::iterator ihis = ihistset->second.begin(); ihis != ihistset->second.end(); ihis++) {
      
      TProfile* ihist = dynamic_cast<TProfile*>(*ihis);
      if ( !ihist ) { continue; }
      
      //look for "run_type id" in the histo title (quick check)
      const string name((ihist)->GetName());
      if (name.find(runTypeId_) != std::string::npos) {
	//extract histogram details from encoded histogram name.
	SiStripHistoTitle h_title = histoTitle(name);
	//update the profile name to the "standard format"
	string newName = SiStripHistoTitle(h_title).title();
	(ihist)->SetName(newName.c_str());
	//add relevent directory for device if required
	client_->addDevice(h_title.keyValue());
	//update map with reformatted profile using histo key (indicating control path) as the index
	commissioning_map[h_title.keyValue()].reserve(6);
	commissioning_map[h_title.keyValue()].push_back(*ihist);
      }
    }
  }

  //Loop commissioning map and:
  //1) On pedestal runs, split the module TProfile into 2 or 3 corresponding LLD channel TProfiles.
  //2) Convert apv numbering scheme for 4-apv modules from 32,33,34,35 to 32,33,36,37. (only relevent for run_types conducted on the apv level).
  //3) Write final "client" profile histogram to file.

  for (map< unsigned int,vector<TProfile> >::iterator ihistset = commissioning_map.begin(); ihistset != commissioning_map.end(); ihistset++) {

    //Change to relevent directory in output file for storage of "client histogram"
    SiStripFecKey c_path(ihistset->first);
    string path = c_path.path();
    TDirectory* ccuChan = client_->dqmTop()->GetDirectory(path.c_str());
 
    if ((runType_ == sistrip::PEDESTALS) && (ihistset->second.size() == 2)) {
     
      for (unsigned short ihisto = 0; ihisto < 2; ihisto++) {

	//split pedestals TProfile
	vector<TProfile> lld_peds;
	lldPedestals(ihistset->second[ihisto],lld_peds);

	//update map
	copy(lld_peds.begin(), lld_peds.end(), back_inserter(ihistset->second));
      }
      //remove old TProfiles
      ihistset->second.erase(ihistset->second.begin(),ihistset->second.begin()+2);
    }

    for (unsigned int iprof = 0; iprof < ihistset->second.size(); iprof++) {
      if (ihistset->second.size() == 4) {

	//unpack name
	string name(ihistset->second[iprof].GetName());
	SiStripHistoTitle h_title(name);

	//fix channel numbers and granularity
	if ((h_title.channel() == 34) | (h_title.channel() == 35)) {
	  SiStripHistoTitle temp( h_title.runType(),
				  h_title.keyType(),
				  h_title.keyValue(),
				  sistrip::APV,
				  h_title.channel()+2,
				  h_title.extraInfo() );
	  const_cast<TProfile*>(&(ihistset->second[iprof]))->SetName(temp.title().c_str());
	}
      }
      //add client histogram to output file
      ccuChan->WriteTObject(&ihistset->second[iprof]);
    }
  }
  return true;
}


//-----------------------------------------------------------------------------

void SiStripTBMonitorToClientConvert::setRunInfo() {

  //Get the run number from each file in list and compare...
  unsigned int istart = tb_path_.find("TBMonitor");
  unsigned int iend = tb_path_.find("_");
  string run = ((istart != string::npos) && (iend != string::npos)) ? tb_path_.substr(istart+9,iend-istart-9) : string("0");
 
  cout << "[SiStripTBMonitorToClientConvert::setRunInfo]: Run number: " << run << endl;
  
  //set run number
  run_ = atoi(run.c_str());
}

//-----------------------------------------------------------------------------

std::string SiStripTBMonitorToClientConvert::runTypeId(sistrip::RunType run_type) {

  std::string run_type_id = "";

  if (run_type == sistrip::PEDESTALS) {/* uses all TProfiles */}
  else if (run_type == sistrip::FED_CABLING) {/* to be set*/}
  else if (run_type == sistrip::VPSP_SCAN) {run_type_id = "vpsp_mean";}
  else if (run_type == sistrip::OPTO_SCAN) {run_type_id = "_gain";}
  else if (run_type == sistrip::APV_TIMING) {run_type_id = "tick_chip";}
  else if (run_type == sistrip::FED_TIMING) {run_type_id = "tickfed_chip";}
  else if (run_type == sistrip::APV_LATENCY) {/* to be set*/}

  else {cout << "[SiStripTBMonitorToClientConvert::runTypeId]: Unknown Commissioning run_type, filling event with ALL TProfile's found in specified files.";}

  return run_type_id;

}

//-----------------------------------------------------------------------------

SiStripHistoTitle SiStripTBMonitorToClientConvert::histoTitle(const string& histo_name) {

  sistrip::RunType run_type = sistrip::UNKNOWN_RUN_TYPE;
  sistrip::KeyType key_type = sistrip::UNKNOWN_KEY;
  uint32_t key_value = sistrip::invalid32_;
  sistrip::Granularity granularity = sistrip::UNKNOWN_GRAN;
  uint16_t channel = sistrip::invalid_;
  std::string extra_info = "";
  
  //scan histogram name
  
  unsigned int start = histo_name.find("0x");
  if (start == std::string::npos) {start = histo_name.find("-") - 1;} //due to variations in TBMonitor histo titling
  
  unsigned int stop = histo_name.find("_", start+2);
  if (stop == std::string::npos) stop = histo_name.find("-", start+2);//due to variations in TBMonitor histo titling

  // Set SiStripEnumsAndStrings::HistoTitle::runType_

  run_type = runType_;
  
  // Set SiStripEnumsAndStrings::HistoTitle::extraInfo_
  
  //extract gain and digital level from histo name if run_type is BIASGAIN
  if (runType_ == sistrip::OPTO_SCAN) {
    if ((histo_name.find("_gain") != std::string::npos) &&
	((histo_name.find("tick") != std::string::npos) | (histo_name.find("base") != std::string::npos))) {
      
      stringstream nm;
      nm << sistrip::gain_ << histo_name.substr((histo_name.size() - 1),1);
      
      if (histo_name.find("tick") != std::string::npos) {
	nm << sistrip::digital_ << 1;}
      else {nm << sistrip::digital_ << 0;}

      extra_info = nm.str();
    }
    else {cout << "[SiStripTBMonitorToClientConvert::histoTitle]: Inconsistency in TBMonitor histogram name for the OPTO_SCAN run_type. One or more of the strings \"gain\", \"tick\" and \"base\" were not found.";}
  }

  if (runType_ ==sistrip::PEDESTALS) {
    string label = histo_name.substr(0,start+1);
    if (label == "Profile_ped") extra_info = sistrip::pedsAndRawNoise_;
    else if (label == "Profile_noi") extra_info = sistrip::residualsAndNoise_;
  }
  
  // Set SiStripEnumsAndStrings::HistoTitle::channel_
  
  channel = (stop != std::string::npos) ? atoi(histo_name.substr(stop+1, 3).c_str()) : 0;//apvnum = 0 if  not specified.
  
  if ((histo_name.size() - stop) == 2) {channel = atoi(histo_name.substr(stop+1, 1).c_str());}//due to variations in TBMonitor histo titling.

  // Set SiStripEnumsAndStrings::HistoTitle::granularity_
  
  if (runType_ == sistrip::VPSP_SCAN) {
    granularity = sistrip::APV;
    channel += 32; // convert "apv number (0-5) to HW address (32-37).
  }
  
  else if ((runType_ == sistrip::OPTO_SCAN) || (runType_ == sistrip::APV_TIMING) || (runType_ == sistrip::FED_TIMING)) { 
    granularity = sistrip::LLD_CHAN;}
  
  else if (runType_ == sistrip::PEDESTALS) {
    granularity = sistrip::MODULE;}
  
  else {cout << "[SiStripTBMonitorToClientConvert::histoTitle]: Unknown Commissioning run_type. Setting SiStripEnumsAndStrings::HistoName::granularity_ to \"UNKNOWN GRANULARITY\".";}

  // Set SiStripEnumsAndStrings::HistoTitle::keyValue_

  stringstream os(""); 
  if (stop != std::string::npos) {
    os << histo_name.substr(start+2,(stop-start-2));}
  else { os << histo_name.substr(start+2);}
  
  unsigned int idlocal;
  os >> hex >> idlocal;

  SiStripFecKey path( 0,
		      ((idlocal>>20)&0xF),
		      ((idlocal>>16)&0xF),
		      ((idlocal>>8)&0xFF),
		      (idlocal&0xFF),
		      (channel&0x3) );
  
  // updates key to the format defined in DataFormats/SiStripDetId/interface/SiStripFecKey.h
  key_value = SiStripFecKey( path.fecCrate(),
			     path.fecSlot(),
			     path.fecRing(),
			     path.ccuAddr(),
			     path.ccuChan(),
			     path.channel() ).key(); 

  //initialise SiStripEnumsAndStrings::HistoTitle object
  SiStripHistoTitle title( run_type,
			   key_type,
			   key_value,
			   granularity,
			   channel,
			   extra_info );

  return title;
}

//-----------------------------------------------------------------------------

void SiStripTBMonitorToClientConvert::lldPedestals(TProfile& module, vector<TProfile>& llds) {
   
  //unpack name
  string name(module.GetName());
  SiStripHistoTitle h_title(name);
  
  unsigned short numApvPairs = module.GetNbinsX()/256;
  llds.reserve(numApvPairs);
  
  for (unsigned short ihisto = 0; ihisto < numApvPairs; ihisto++) {
     
    //get lld channel number
    unsigned short illd = ((numApvPairs == 2) && (ihisto == 1)) ? illd = 2 : ihisto;
    //create and format new TProfile
    TProfile apvPeds;
    apvPeds.SetBins(256, 0., 256.);
    apvPeds.SetName(h_title.title().c_str());
    //fill new TProfile
    for (unsigned short ibin = 0; ibin < apvPeds.GetNbinsX(); ibin++) {

      // Set values for a TProfile bin
      UpdateTProfile::setBinContent( &apvPeds,(uint32_t)(ibin+1), 
				     (double)module.GetBinEntries((Int_t)(ihisto*256 + ibin + 1)), 
				     (double)module.GetBinContent((Int_t)(ihisto*256 + ibin + 1)), 
				     (double)module.GetBinError((Int_t)(ihisto*256 + ibin + 1)) );
    }
    
    //add new TProfile to llds vector
    llds.push_back(apvPeds);}
}
