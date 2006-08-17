#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/FedTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/OptoScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/VpspScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvLatencyAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/PedestalsAnalysis.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#define DBG "FILE: " << __FILE__ << "\n" << "FUNC: " << __PRETTY_FUNCTION__ 

using namespace std;

// -----------------------------------------------------------------------------
//
SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient( string file, 
								      string level ) :
  file_(file),
  summary_path_(""),
  task_(sistrip::APV_TIMING),
  view_(sistrip::CONTROL),
  level_(level),
//   c_summary_(0),
//   c_summary2_(0),
  summary_(0),
  client_(0),
  run_(0),
  map_()
{

  // Open client file
  client_ = new SiStripCommissioningFile( file_.c_str() );
  //task_ = client_->Task();
  //view_ = client_->View();
  
  if ( !client_->queryDQMFormat() ) { 
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Error when reading file: " 
	 << file_ 
	 << endl;
  }
  if ( task_ == sistrip::UNKNOWN_TASK ) { 
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unknown commissioning task: " 
	 << SiStripHistoNamingScheme::task( task_ ) 
	 << endl;
  }
  if ( view_ == sistrip::UNKNOWN_VIEW ) {
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unknown view: " 
	 << SiStripHistoNamingScheme::view( view_ ) 
	 << endl;
  }
  
  map_.clear();
  
  setRunNumber();
  //createSummaryHisto();
  
}

//-----------------------------------------------------------------------------
//
SiStripOfflineCommissioningClient::~SiStripOfflineCommissioningClient() {
  //writeSummary();
  if ( client_ ) { delete client_; }
  //if ( map_ ) { delete map_; } // delete histo pointers?
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::analysis() {
  
  // Fill map with commissioning histograms 
  fillMap();

  if ( task_ == sistrip::APV_TIMING ) { apvTiming(); }
  else { /* do nothing yet... */ }
  
  // Temporary storage for multi-histogram based analysis
  map< uint32_t, vector< vector<TProfile*> > > histo_organizer;

  // loop over histograms
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    
//     // update summary file with a directory for this device
//     summary_->addDevice( imap->first );

    Histos::const_iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {
 
//       // Extract histogram details from encoded histogram name.
//       SiStripHistoNamingScheme::HistoTitle title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
//       SiStripControlKey::ControlPath path = SiStripControlKey::path( imap->first );
      
//       if ( task_ == sistrip::APV_TIMING ) {
	
// 	ApvTimingAnalysis::Monitorables mons;
// 	ApvTimingAnalysis::analysis( *ihis, mons );

// 	// 	stringstream ss;
// 	// 	ss << "[" << __PRETTY_FUNCTION__ << "]" 
// 	// 	   << " Title: " << (*ihis)->GetName()
// 	// 	   << " Key: " << imap->first
// 	// 	   << " Crate/Slot/Ring/CCU/Module/Channel: " 
// 	// 	   << path.fecCrate_ << "/" 
// 	// 	   << path.fecSlot_ << "/" 
// 	// 	   << path.fecRing_ << "/" 
// 	// 	   << path.ccuAddr_ << "/" 
// 	// 	   << path.ccuChan_ << "/" 
// 	// 	   << title.channel_ << "\n";
// 	// 	mons.print(ss);
// 	// 	cout << ss.str() << endl;

//       }
      
//       else if ( task_ == sistrip::PEDESTALS ) {
	
// 	//fill map with module histograms using key
// 	if (histo_organizer.find(h_title.channel_) == histo_organizer.end()) {
// 	  histo_organizer[h_title.channel_] = vector< vector<TProfile*> >(1, vector<TProfile*>(2,(TProfile*)(0)));}
	
// 	if (h_title.extraInfo_.find(sistrip::pedsAndRawNoise_) != string::npos) {histo_organizer[h_title.channel_][0][0] = const_cast<TProfile*>(*prof);}
// 	else if (h_title.extraInfo_.find(sistrip::residualsAndNoise_)  != string::npos) {histo_organizer[h_title.channel_][0][1] = const_cast<TProfile*>(*prof);}
	
// 	//if last histo in vector (i.e. for module) perform analysis and add to summary....
// 	if (prof == (ihistomap->second.end() - 1)) {
	  
// 	  //define analysis object
// 	  PedestalsAnalysis anal;
	  
// 	  //loop over histograms
// 	  for (map< unsigned int, vector< vector< TProfile* > > >::iterator it = histo_organizer.begin(); it != histo_organizer.end(); it++) {
	    
// 	    vector<const TProfile*> c_histos; 
// 	    c_histos.push_back(it->second[0][0]); c_histos.push_back(it->second[0][1]);
// 	    vector< vector<float> > c_monitorables;
// 	    anal.analysis(c_histos, c_monitorables);
	    
// 	    //ped == average pedestals, noise == average noise
// 	    float ped = 0, noise = 0;
	    
// 	    if (c_monitorables[0].size() == c_monitorables[1].size() != 0) {
// 	      for (unsigned short istrip = 0; istrip < c_monitorables[0].size(); istrip++) {
// 		ped += c_monitorables[0][istrip];
// 		noise += c_monitorables[1][istrip];
// 	      }
// 	      ped = ped/c_monitorables[0].size();
// 	      noise = noise/c_monitorables[0].size();
// 	    }
	    
// 	    //update summary
// 	    c_summary_->update(ihistomap->first, ped); 
// 	    c_summary2_->update(ihistomap->first, noise);
// 	  }
// 	  histo_organizer.clear();//refresh the container
// 	}
//       }
      
//       else if ( task_ == sistrip::VPSP_SCAN ) {
	
// 	VpspScanAnalysis anal;
	
// 	vector<const TProfile*> c_histos;
// 	c_histos.push_back(*prof);
// 	vector<unsigned short> c_monitorables;
// 	anal.analysis(c_histos, c_monitorables);
// 	unsigned int val = c_monitorables[0];
// 	c_summary_->update(ihistomap->first, val); 
	
//       }
      
//       else if ( task_ == sistrip::FED_TIMING ) {
	
// 	FedTimingAnalysis anal;
	
// 	vector<const TProfile*> c_histos;
// 	c_histos.push_back(*prof);
// 	vector<unsigned short> c_monitorables;
// 	anal.analysis(c_histos, c_monitorables);
// 	unsigned int val = c_monitorables[0] * 25 + c_monitorables[1];
// 	c_summary_->update(ihistomap->first, val); 
	
//       }
      
//       else if ( task_ == sistrip::OPTO_SCAN ) {

// 	//find gain value + digital level.
// 	string::size_type index = h_title.extraInfo_.find(sistrip::gain_);
// 	unsigned short gain = atoi(h_title.extraInfo_.substr((index + 4),1).c_str());

// 	index = h_title.extraInfo_.find(sistrip::digital_);
// 	unsigned short digital = atoi(h_title.extraInfo_.substr((index + 7),1).c_str());

// 	//fill map with module histograms using key
// 	if (histo_organizer.find(h_title.channel_) == histo_organizer.end()) {
// 	  histo_organizer[h_title.channel_] = vector< vector<TProfile*> >(4, vector<TProfile*>(2,(TProfile*)(0)));}
	
// 	if (digital == 0) {
// 	  histo_organizer[h_title.channel_][gain][0] = const_cast<TProfile*>(*prof);}
	
// 	if (digital == 1) {
// 	  histo_organizer[h_title.channel_][gain][1] = const_cast<TProfile*>(*prof);}
	
// 	//if last histo in vector (i.e. for channel) perform analysis....
// 	if (prof == (ihistomap->second.end() - 1)) {
	  
// 	  OptoScanAnalysis anal;
// 	  vector<float> c_monitorables; c_monitorables.resize(2,0.);
	  
// 	  //loop over lld channels
// 	  for (map< unsigned int, vector< vector< TProfile* > > >::iterator it = histo_organizer.begin(); it != histo_organizer.end(); it++) {
	    
// 	    //loop over histos for of a single lld channel (loop over gain)
// 	    for (unsigned short igain = 0; igain < it->second.size(); igain++) {
	      
// 	      if (it->second[igain][0] && it->second[igain][1]) {
// 		vector<const TProfile*> c_histos; 
// 		c_histos.push_back(it->second[igain][0]);
// 		c_histos.push_back(it->second[igain][1]);
// 		vector<float> temp_monitorables;
// 		anal.analysis(c_histos, temp_monitorables);
		
// 		//store monitorables with gain nearest target.
// 		if ((fabs(temp_monitorables[0] - targetGain_) < fabs(c_monitorables[0] - targetGain_)) || ((it == histo_organizer.begin()) && igain == 0)) {c_monitorables = temp_monitorables;}
// 	      }
// 	    }

// 	    c_summary_->update(ihistomap->first, c_monitorables[1]);
// 	    c_summary2_->update(ihistomap->first, c_monitorables[0]); 
// 	  }
// 	  histo_organizer.clear();
// 	}
//       }
      
//       else if ( task_ == sistrip::APV_LATENCY ) {
	
// 	ApvLatencyAnalysis anal;
	
// 	vector<const TProfile*> c_histos;
// 	c_histos.push_back(*prof);
// 	vector<unsigned short> c_monitorables;
// 	anal.analysis(c_histos, c_monitorables);
// 	unsigned int val = c_monitorables[0];
// 	c_summary_->update(ihistomap->first, val); 
//       }
      
//  else {
// 	cout << "[" << __PRETTY_FUNCTION__ << "]"
// 	     << " Unknown task: " << task_ << endl; 
// 	return;
//       }
    }

  }

}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::setRunNumber() {
  
  // Extract run number from client name
  uint32_t ii = file_.find("_");
  uint32_t jj = file_.find(".root");
  if ( ii == string::npos || jj == string::npos ) { run_ = 0; }
  else { run_ = static_cast<uint16_t>( atoi( file_.substr(ii+1,jj).c_str()) ); }
  
  if ( !run_ ) {
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Run number set to zero!"
	 << " File name should have form client_xxxxx.root'"
	 << ", where 'xxxxx' is run number" << endl;
  }
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::createSummaryHisto() {

//   const char* title;
//   const char* name;
  
//   if (task_ == sistrip::OPTO_SCAN) {
//     c_summary_ = new CommissioningSummary(sistrip::CONTROL);
//     title = name = ((string)("Bias")).c_str();
//     c_summary_->setName(name);
//     c_summary_->setTitle(title);
    
//     c_summary2_ = new CommissioningSummary(sistrip::CONTROL);
//     title = name = ((string)("Gain")).c_str();
//     c_summary2_->setName(name);
//     c_summary2_->setTitle(title);
//   }
  
//   else if (task_ == sistrip::PEDESTALS) {
//     c_summary_ = new CommissioningSummary(sistrip::CONTROL);
//     title = name = ((string)("Pedestals")).c_str();
//     c_summary_->setName(name);
//     c_summary_->setTitle(title);

//     c_summary2_ = new CommissioningSummary(sistrip::CONTROL);
//     title = name = ((string)("Noise")).c_str();
//     c_summary2_->setName(name);
//     c_summary2_->setTitle(title);
//   }

//   else {
//     c_summary_ = new CommissioningSummary(sistrip::CONTROL);
//     title = name = SiStripHistoNamingScheme::task(task_).c_str();
//     c_summary_->setName(name);
//     c_summary_->setTitle(title);
//   }
  
//  //Construct and name summary file...
//   stringstream ss; 
//   ss << summary_path_.substr( 0, summary_path_.find(".root",0)) 
//      << "_" 
//      << SiStripHistoNamingScheme::task(task_) 
//      << "_" 
//      << setfill('0') 
//      << setw(7) 
//      << run_ 
//      << ".root";

//   summary_ = new SiStripCommissioningFile(ss.str().c_str(), "RECREATE");
//   summary_->setDQMFormat(task_,view_);
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::fillMap() {
  
  map< string, vector<TProfile*> > profile_map;
  
  // Get histograms from client file
  client_->findProfiles( client_->top(), &profile_map );
  
  // Convert map (to use FEC key as index, rather than directory string)
  convertMap( profile_map );

}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::convertMap( map< string, vector<TProfile*> >& histos ) {

  map< string, vector<TProfile*> >::iterator iter = histos.begin();
  for ( ; iter != histos.end(); iter++ ) {
    
    uint32_t index = iter->first.find( sistrip::controlView_ );
    string control = iter->first.substr( index );
    SiStripHistoNamingScheme::ControlPath path = SiStripHistoNamingScheme::controlPath( control );
    
    vector<TProfile*>::iterator ihis = iter->second.begin();
    for ( ; ihis != iter->second.end(); ihis++ ) {

      SiStripHistoNamingScheme::HistoTitle h_title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      uint16_t key = SiStripControlKey::key( path.fecCrate_, 
					     path.fecSlot_, 
					     path.fecRing_, 
					     path.ccuAddr_, 
					     path.ccuChan_,
					     h_title.channel_ );
      map_[key].push_back(*ihis);
      
    }
  }
  
}
  
//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::writeSummaryHistoToFile() {

//   //write summary histogram(s) to file
//   stringstream histo_name;

//   if (c_summary_) {
//     c_summary_->histogram(level_);
//     summary_->sistripTop()->cd(level_.c_str());
//     c_summary_->getSummary()->Write();
//     c_summary_->getHistogram()->Write();}
  
//   if (c_summary2_) {
//     c_summary2_->histogram(level_);
//     summary_->sistripTop()->cd(level_.c_str());
//     c_summary2_->getSummary()->Write();
//     c_summary2_->getHistogram()->Write();
//   }

//   if (c_summary_) delete c_summary_;
//   if (c_summary2_) delete c_summary2_;
//   if (summary_) delete summary_;

}


//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::apvTiming() {
  cout << DBG << endl;
  
  // Storage for monitorables
  map<uint32_t,ApvTimingAnalysis::Monitorables> monitorables;
  
  // loop over histograms
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    Histos::const_iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {
      
      // Do histo analysis and create monitorables object
      ApvTimingAnalysis::Monitorables mons;
      ApvTimingAnalysis::analysis( *ihis, mons );
      monitorables[imap->first] = mons;
      
      //       // Extract histogram details from encoded histogram name
      //       SiStripHistoNamingScheme::HistoTitle title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      //       SiStripControlKey::ControlPath path = SiStripControlKey::path( imap->first );
      //       stringstream ss;
      //       ss << "[" << __PRETTY_FUNCTION__ << "]" 
      // 	 << " Title: " << (*ihis)->GetName()
      // 	 << " Key: " << imap->first
      // 	 << " Crate/Slot/Ring/CCU/Module/Channel: " 
      // 	 << path.fecCrate_ << "/" 
      // 	 << path.fecSlot_ << "/" 
      // 	 << path.fecRing_ << "/" 
      // 	 << path.ccuAddr_ << "/" 
      // 	 << path.ccuChan_ << "/" 
      // 	 << title.channel_ << "\n";
      //       mons.print(ss);
      //       cout << ss.str() << endl;
      
    }
  }
  
  // Some debug
  cout << " Complete list of APV timing delays [ns] for " 
       << monitorables.size() << " modules: ";
  map<uint32_t,ApvTimingAnalysis::Monitorables>::const_iterator iter;
  for ( iter = monitorables.begin(); iter != monitorables.end(); iter++ ) {
    cout << iter->second.delay_ << ", ";
  }
  cout << endl;
  
}
