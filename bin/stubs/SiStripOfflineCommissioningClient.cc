#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h"
//common
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
///analysis
#include "DQM/SiStripCommissioningAnalysis/interface/ApvTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/FedTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/OptoScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/VpspScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvLatencyAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/PedestalsAnalysis.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient(string client_path, string summary_path, string summaryLevel) :

  //initialise private data members
  client_path_(client_path),
  summary_path_(summary_path),
  task_(sistrip::UNKNOWN_TASK),
  view_(sistrip::UNKNOWN_VIEW),
  dirLevel_(summaryLevel),
  c_summary_(0),
  c_summary2_(0),
  summary_(0),
  client_(0),
  targetGain_(0.8),
  run_(0),
  commissioning_map_(0)
  
{
  //Open client file
  client_ = new SiStripCommissioningFile(client_path_.c_str());
  task_ = client_->Task();
  view_ = client_->View();

 //Check Commissioning Task and Readout View
  if (task_ == sistrip::UNKNOWN_TASK) cout << "[SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient]: Unknown commissioning task. Value used: " << task_ << "; values accepted: Pedestals, ApvTiming, FedTiming, OptoScan, VpspScan, ApvLatency." << endl;
  if (view_ == sistrip::UNKNOWN_VIEW) cout << "[SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient]: Unknown readout view. Value used: " << view_ << "; values accepted: ControlView." << endl;

  if (!client_->queryDQMFormat()) cout << "[SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient]: Error when reading file: " << client_path_ << ". Not interpretted as \"DQM\" format." << endl;

  //initialize commissioning map
  commissioning_map_ = new map< unsigned int,vector<const TProfile*> >();

  setRunInfo();

  prepareSummary();
}

//-----------------------------------------------------------------------------

SiStripOfflineCommissioningClient::~SiStripOfflineCommissioningClient() {

  writeSummary();

  //clean-up
  if (client_) delete client_;
  if (commissioning_map_) delete commissioning_map_;
}

//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::analysis() {
  
  // Fill private data member commissioning_map_ using the contents of SiStripCommissioningFile client_
  fillCommissioningMap();
  
 //storage tool for multi-histogram based analysis
  map< unsigned int, vector< vector< TProfile* > > > histo_organizer;

  //loop over histograms
  for (map<unsigned int, vector<const TProfile*> >::const_iterator ihistomap = commissioning_map_->begin(); ihistomap != commissioning_map_->end(); ihistomap++) {

    //update summary file with a directory for this device
    summary_->addDevice(ihistomap->first);

    for (vector<const TProfile*>::const_iterator prof = ihistomap->second.begin(); prof != ihistomap->second.end(); prof++) {
 
      //extract histogram details from encoded histogram name.
      std::string name((*prof)->GetName());
      SiStripHistoNamingScheme::HistoTitle h_title = SiStripHistoNamingScheme::histoTitle(name);
 
      //find control path from map key
      SiStripControlKey::ControlPath path = SiStripControlKey::path(ihistomap->first);
      
      //get module information for the summary
      CommissioningSummary::ReadoutId readout(ihistomap->first, h_title.channel_);
      
      //commissioning analysis
      
      if (task_ == sistrip::APV_TIMING) {
	
	ApvTimingAnalysis anal;
	
	vector<const TProfile*> c_histos;
	c_histos.push_back(*prof);
	vector<unsigned short> c_monitorables;
	anal.analysis(c_histos, c_monitorables);
	unsigned int val = c_monitorables[0] * 24 + c_monitorables[1];
	c_summary_->update(readout, val); 
      }
      
      else if (task_ == sistrip::PEDESTALS) {

	//fill map with module histograms using key
	if (histo_organizer.find(h_title.channel_) == histo_organizer.end()) {
	  histo_organizer[h_title.channel_] = vector< vector<TProfile*> >(1, vector<TProfile*>(2,(TProfile*)(0)));}

	if (h_title.extraInfo_.find(sistrip::pedsAndRawNoise_) != string::npos) {histo_organizer[h_title.channel_][0][0] = const_cast<TProfile*>(*prof);}
	else if (h_title.extraInfo_.find(sistrip::residualsAndNoise_)  != string::npos) {histo_organizer[h_title.channel_][0][1] = const_cast<TProfile*>(*prof);}

	//if last histo in vector (i.e. for module) perform analysis and add to summary....
	if (prof == (ihistomap->second.end() - 1)) {

	  //define analysis object
	PedestalsAnalysis anal;

	//loop over lld channels
	for (map< unsigned int, vector< vector< TProfile* > > >::iterator it = histo_organizer.begin(); it != histo_organizer.end(); it++) {
	vector<const TProfile*> c_histos;
	c_histos.push_back(it->second[0][0]); c_histos.push_back(it->second[0][1]);
	vector< vector<float> > c_monitorables;
	anal.analysis(c_histos, c_monitorables);
       
	//ped == average pedestals, noise == average noise
	float ped = 0, noise = 0;
	
	if (c_monitorables[0].size() == c_monitorables[1].size() != 0) {
	  for (unsigned short istrip = 0; istrip < c_monitorables[0].size(); istrip++) {
	    ped += c_monitorables[0][istrip];
	    noise += c_monitorables[1][istrip];
	  }
	  ped = ped/c_monitorables[0].size();
	  noise = noise/c_monitorables[0].size();
	}
	
	//update summary
	CommissioningSummary::ReadoutId readout(ihistomap->first, it->first);
	c_summary_->update(readout, ped); 
	c_summary2_->update(readout, noise);
	}
	histo_organizer.clear();//refresh the container
	}
      }
      
      else if (task_ == sistrip::VPSP_SCAN) {
	
	VpspScanAnalysis anal;
	
	vector<const TProfile*> c_histos;
	c_histos.push_back(*prof);
	vector<unsigned short> c_monitorables;
	anal.analysis(c_histos, c_monitorables);
	unsigned int val = c_monitorables[0];
	c_summary_->update(readout, val); 
	
      }
      
      else if (task_ == sistrip::FED_TIMING) {
	
	FedTimingAnalysis anal;
	
	vector<const TProfile*> c_histos;
	c_histos.push_back(*prof);
	vector<unsigned short> c_monitorables;
	anal.analysis(c_histos, c_monitorables);
	unsigned int val = c_monitorables[0] * 25 + c_monitorables[1];
	c_summary_->update(readout, val); 
	
      }
      
      else if (task_ == sistrip::OPTO_SCAN) {

	//find gain value + digital level.
	string::size_type index = h_title.extraInfo_.find(sistrip::gain_);
	unsigned short gain = atoi(h_title.extraInfo_.substr((index + 4),1).c_str());

	index = h_title.extraInfo_.find(sistrip::digital_);
	unsigned short digital = atoi(h_title.extraInfo_.substr((index + 7),1).c_str());

	//fill map with module histograms using key
	if (histo_organizer.find(h_title.channel_) == histo_organizer.end()) {
	  histo_organizer[h_title.channel_] = vector< vector<TProfile*> >(4, vector<TProfile*>(2,(TProfile*)(0)));}
	
	if (digital == 0) {
	  histo_organizer[h_title.channel_][gain][0] = const_cast<TProfile*>(*prof);}
	
	if (digital == 1) {
	  histo_organizer[h_title.channel_][gain][1] = const_cast<TProfile*>(*prof);}
	
	//if last histo in vector (i.e. for module) perform analysis....
	if (prof == (ihistomap->second.end() - 1)) {
	  
	  OptoScanAnalysis anal;
	  vector<float> c_monitorables; c_monitorables.resize(2,0.);
	  
	  //loop over lld channels
	  for (map< unsigned int, vector< vector< TProfile* > > >::iterator it = histo_organizer.begin(); it != histo_organizer.end(); it++) {
	    
	    //loop over histos for of a single lld channel (loop over gain)
	    for (unsigned short igain = 0; igain < it->second.size(); igain++) {
	      
	      if (it->second[igain][0] && it->second[igain][1]) {
		vector<const TProfile*> c_histos; 
		c_histos.push_back(it->second[igain][0]);
		c_histos.push_back(it->second[igain][1]);
		vector<float> temp_monitorables;
		anal.analysis(c_histos, temp_monitorables);
		
		//store monitorables with gain nearest target.
		if ((fabs(temp_monitorables[0] - targetGain_) < fabs(c_monitorables[0] - targetGain_)) || ((it == histo_organizer.begin()) && igain == 0)) {c_monitorables = temp_monitorables;}
	      }
	    }
	    
	    CommissioningSummary::ReadoutId readout(ihistomap->first, it->first);
	    c_summary_->update(readout, c_monitorables[1]);
	    c_summary2_->update(readout, c_monitorables[0]); 
	  }
	  histo_organizer.clear();
	}
      }
      
      else if (task_ == sistrip::APV_LATENCY) {
	
	ApvLatencyAnalysis anal;
	
	vector<const TProfile*> c_histos;
	c_histos.push_back(*prof);
	vector<unsigned short> c_monitorables;
	anal.analysis(c_histos, c_monitorables);
	unsigned int val = c_monitorables[0];
	c_summary_->update(readout, val); 
      }
      
      else {cout << "[SiStripOfflineCommissioningClient::analysis]: Task \"" << task_ << "\" not recognized."; return;}
    }
  }
}

//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::setRunInfo() {

 // Extract run number from client name
  unsigned int istart = client_path_.find("Client_");
  unsigned int iext = client_path_.find(".root");
  std::string run = ((istart != string::npos) && (iext != string::npos)) ? client_path_.substr((istart+7),(iext-istart-7)) : string("0");
  run_ = (unsigned short)(atoi(run.c_str()));
  
  if (!run_) {cout << "[SiStripOfflineCommissioningClient::begin]:Run number eq 0. Check Client file name has the form Client_(run number).root: " << endl;}
}

//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::prepareSummary() {

 //construct summary objects as necessary
  if (task_ == sistrip::VPSP_SCAN) {
    c_summary_ = new CommissioningSummary(SiStripHistoNamingScheme::task(task_), sistrip::APV);}

  else if (task_ == sistrip::OPTO_SCAN) {
    c_summary_ = new CommissioningSummary((string)("Bias"), sistrip::LLD_CHAN);
    c_summary2_ = new CommissioningSummary((string)("Gain"), sistrip::LLD_CHAN);}
  
  else if (task_ == sistrip::PEDESTALS) {
    c_summary_ = new CommissioningSummary((string)("Pedestals"), sistrip::LLD_CHAN);
    c_summary2_ = new CommissioningSummary((string)("Noise"), sistrip::LLD_CHAN);
  }

  else {c_summary_ = new CommissioningSummary(SiStripHistoNamingScheme::task(task_), sistrip::LLD_CHAN);}
  
 //Construct and name summary file...
  string name = summary_path_.substr( 0, summary_path_.find(".root",0));
  stringstream ss; ss << name << "_" << SiStripHistoNamingScheme::task(task_) << "_" << setfill('0') << setw(7) << run_ << ".root";
  summary_ = new SiStripCommissioningFile(ss.str().c_str(), "RECREATE");
  summary_->setDQMFormat(task_,view_);
 
}

//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::fillCommissioningMap() {

 //Initial histogram container
  map< string, vector<TProfile*> > profile_map;
  
  //Get histograms from client file
  client_->findProfiles(client_->top(),&profile_map);
  
  //Convert map indexed by directory string to "commissioning map" (indexed by fec-key).
  convertMap(&profile_map);
}

//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::convertMap(map< string, vector<TProfile*> >* directory_map) {


 //Loop all commissioning tprofiles. Fill map, indexing with CCU Address fec-key. Update client file directory structure.
  
  for (map< string, vector<TProfile*> >::iterator ihistset = directory_map->begin(); ihistset != directory_map->end(); ihistset++) {
    
    for (vector<TProfile*>::iterator ihist = ihistset->second.begin(); ihist != ihistset->second.end(); ihist++) {
      
      //extract histogram details from encoded histogram name.
      const string name((*ihist)->GetName());
      SiStripHistoNamingScheme::HistoTitle h_title = SiStripHistoNamingScheme::histoTitle(name);
      
      //Find fec key.......
      string::size_type index = ihistset->first.find(sistrip::controlView_);
      string control = ihistset->first.substr(index);
      
      SiStripHistoNamingScheme::ControlPath fec_path = SiStripHistoNamingScheme::controlPath(control);
      unsigned int fec_key = SiStripControlKey::key(fec_path.fecCrate_, fec_path.fecSlot_, fec_path.fecRing_, fec_path.ccuAddr_, fec_path.ccuChan_);
 
      //update map...
      (*commissioning_map_)[fec_key].reserve(6);
      (*commissioning_map_)[fec_key].push_back(*ihist);
      
    }
  }
}
  
//-----------------------------------------------------------------------------

void SiStripOfflineCommissioningClient::writeSummary() {

  //write summary histogram(s) to file
  stringstream histo_name;

  if (c_summary_) {
    TH1F* control = c_summary_->controlSummary(dirLevel_);
    histo_name << control->GetName() << "_Control";
    control->SetName(histo_name.str().c_str());
    TH1F* global =  c_summary_->summary(dirLevel_);
    summary_->sistripTop()->cd(dirLevel_.c_str());
    control->Write();
    global->Write();
  }
  
  if (c_summary2_) {
    TH1F* control = c_summary2_->controlSummary(dirLevel_);
    histo_name.str("");
    histo_name << control->GetName() << "_Control";
    control->SetName(histo_name.str().c_str());
    TH1F* global =  c_summary2_->summary(dirLevel_);
    summary_->sistripTop()->cd(dirLevel_.c_str());
    control->Write();
    global->Write();
  }
  summary_->Close();

  if (c_summary_) delete c_summary_;
  if (c_summary2_) delete c_summary2_;
  if (summary_) delete summary_;
}

//-----------------------------------------------------------------------------

