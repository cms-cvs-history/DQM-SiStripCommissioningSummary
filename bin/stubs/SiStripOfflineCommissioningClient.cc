#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineCommissioningClient.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/FedTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/OptoScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/VpspScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvLatencyAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/PedestalsAnalysis.h"
#include "DQM/SiStripCommissioningSummary/interface/ApvTimingSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/OptoScanSummaryFactory.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#define DBG "FILE: " << __FILE__ << "\n" << "FUNC: " << __PRETTY_FUNCTION__ 

using namespace std;

// -----------------------------------------------------------------------------
//
SiStripOfflineCommissioningClient::SiStripOfflineCommissioningClient( string filename,
								      sistrip::SummaryHisto histo,
								      sistrip::SummaryType type,
								      string level ) 
  : filename_(filename),
    task_(sistrip::UNKNOWN_TASK),
    view_(sistrip::UNKNOWN_VIEW),
    histo_(histo),
    type_(type),
    level_(level),
    file_(0),
    run_(0),
    map_()
{

  // Open client file
  file_ = new SiStripCommissioningFile( filename_.c_str() );
  task_ = file_->Task(); 
  view_ = file_->View(); 
  
  if ( !file_->queryDQMFormat() ) { 
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Error when reading file: " 
	 << filename_ 
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
  analysis();
  
}

//-----------------------------------------------------------------------------
//
SiStripOfflineCommissioningClient::~SiStripOfflineCommissioningClient() {
  if ( file_ ) { delete file_; }
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::analysis() {
  
  // Fill map with commissioning histograms 
  fillMap();

  if      ( task_ == sistrip::APV_TIMING ) { apvTiming(); }
  else if ( task_ == sistrip::OPTO_SCAN )  { optoScan(); }
  else { /* do nothing yet... */ }
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::fillMap() {
  
  map< string, vector<TProfile*> > histos;
  
  // Get histograms from client file
  file_->findProfiles( file_->top(), &histos );
  
  // Convert map (to use FEC key as index, rather than directory string)
  map< string, vector<TProfile*> >::iterator iter = histos.begin();
  for ( ; iter != histos.end(); iter++ ) {
    cout << "PATH: " << iter->first << endl;
    uint32_t index = iter->first.find( sistrip::controlView_ );
    string control = iter->first.substr( index );
    SiStripHistoNamingScheme::ControlPath path = SiStripHistoNamingScheme::controlPath( control );
    vector<TProfile*>::iterator ihis = iter->second.begin();
    for ( ; ihis != iter->second.end(); ihis++ ) {
      cout << "NAME: " << (*ihis)->GetName() << endl;
      SiStripHistoNamingScheme::HistoTitle h_title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      uint32_t key = SiStripControlKey::key( path.fecCrate_, 
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
  
  // Create summary histogram
  SummaryHistogramFactory<ApvTimingAnalysis::Monitorables> factory;
  string name = factory.name( histo_, 
			      type_, 
			      view_, 
			      level_ );
  TH1F* summary = new TH1F();
  summary->SetName( name.c_str() );
  factory.generate( histo_,
		    type_,
		    view_, 
		    level_, 
		    monitorables,
		    *summary );
  
  // Write histo to file
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::fedTiming() {
  cout << DBG << endl;
  
  // Storage for monitorables
  map<uint32_t,FedTimingAnalysis::Monitorables> monitorables;
  
  // loop over histograms
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    Histos::const_iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {
      // Do histo analysis and create monitorables object
      FedTimingAnalysis::Monitorables mons;
      FedTimingAnalysis::analysis( *ihis, mons );
      monitorables[imap->first] = mons;
    }
  }
  
  // Some debug
  cout << " Complete list of FED timing delays [ns] for " 
       << monitorables.size() << " modules: ";
  map<uint32_t,FedTimingAnalysis::Monitorables>::const_iterator iter;
  for ( iter = monitorables.begin(); iter != monitorables.end(); iter++ ) {
    cout << iter->second.delay_ << ", ";
  }
  cout << endl;
  
  // Create summary histogram
  SummaryHistogramFactory<FedTimingAnalysis::Monitorables> factory;
  string name = factory.name( histo_, 
			      type_, 
			      view_, 
			      level_ );
  TH1F* summary = new TH1F();
  summary->SetName( name.c_str() );
  factory.generate( histo_,
		    type_,
		    view_, 
		    level_, 
		    monitorables,
		    *summary );
  
  // Write histo to file
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::pedestals() {
  cout << DBG << endl;
  
  // Storage for monitorables
  map<uint32_t,PedestalsAnalysis::Monitorables> monitorables;
  
  // Iterate through map
  HistosMap::iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {

    // Iterate through vector of histos
    PedestalsAnalysis::TProfiles profs;
    Histos::iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {

      if ( !(*ihis) ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " NULL pointer to histogram!" << endl;
	continue;
      }

      // Retrieve control key
      static SiStripHistoNamingScheme::HistoTitle title;
      title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      
      // Some checks
      if ( title.task_ != sistrip::OPTO_SCAN ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected commissioning task!"
	     << "(" << SiStripHistoNamingScheme::task( title.task_ ) << ")"
	     << endl;
      }

      // Extract peds and noise histos
      if ( title.extraInfo_.find(sistrip::pedsAndRawNoise_) != string::npos ) {
	profs.peds_ = *ihis;
      } else if ( title.extraInfo_.find(sistrip::residualsAndNoise_) != string::npos ) {
	profs.noise_ = *ihis;
      } else { 
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected 'extra info': " << title.extraInfo_ << endl;
      }
      
    }
    
    // Perform histo analysis
    PedestalsAnalysis::Monitorables mons;
    PedestalsAnalysis::analysis( profs, mons );

  }

  // Some debug
  cout << " Complete list of mean peds (APV0) for "
       << monitorables.size() << " modules: ";
  map<uint32_t,PedestalsAnalysis::Monitorables>::const_iterator iter;
  for ( iter = monitorables.begin(); iter != monitorables.end(); iter++ ) {
    cout << iter->second.pedsMean_[0] << ", ";
  }
  cout << endl;
  
  // Create summary histogram
  SummaryHistogramFactory<PedestalsAnalysis::Monitorables> factory;
  string name = factory.name( histo_, 
			      type_, 
			      view_, 
			      level_ );
  TH1F* summary = new TH1F();
  summary->SetName( name.c_str() );
  factory.generate( histo_,
		    type_,
		    view_, 
		    level_, 
		    monitorables,
		    *summary );
  
  // Write histo to file
  file_->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::optoScan() {
  cout << DBG << endl;
  
  // Storage for monitorables
  map<uint32_t,OptoScanAnalysis::Monitorables> monitorables;
  
  // Iterate through map
  HistosMap::iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {

    // Iterate through vector of histos
    OptoScanAnalysis::TProfiles profs;
    Histos::iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {

      if ( !(*ihis) ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " NULL pointer to histogram!" << endl;
	continue;
      }

      // Retrieve control key
      static SiStripHistoNamingScheme::HistoTitle title;
      title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      
      // Some checks
      if ( title.task_ != sistrip::OPTO_SCAN ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected commissioning task!"
	     << "(" << SiStripHistoNamingScheme::task( title.task_ ) << ")"
	     << endl;
      }

      // Extract gain setting and digital high/low info
      uint16_t gain = sistrip::invalid_; 
      if ( title.extraInfo_.find(sistrip::gain_) != string::npos ) {
	stringstream ss;
	ss << title.extraInfo_.substr( title.extraInfo_.find(sistrip::gain_) + sistrip::gain_.size(), 1 );
	ss >> dec >> gain;
      }
      uint16_t digital = sistrip::invalid_; 
      if ( title.extraInfo_.find(sistrip::digital_) != string::npos ) {
	stringstream ss;
	ss << title.extraInfo_.substr( title.extraInfo_.find(sistrip::digital_) + sistrip::digital_.size(), 1 );
	ss >> dec >> digital;
      }

      // Store histo pointers
      if ( digital == 0 ) { 
	if      ( gain == 0 ) { profs.g0d0_ = *ihis; }
	else if ( gain == 1 ) { profs.g1d0_ = *ihis; }
	else if ( gain == 2 ) { profs.g2d0_ = *ihis; }
	else if ( gain == 3 ) { profs.g3d0_ = *ihis; }
	else {
	  cerr << "[" << __PRETTY_FUNCTION__ << "]"
	       << " Unexpected gain setting! (" << gain << ")" << endl;
	}
      } else if ( digital == 1 ) { 
	if      ( gain == 0 ) { profs.g0d1_ = *ihis; }
	else if ( gain == 1 ) { profs.g1d1_ = *ihis; }
	else if ( gain == 2 ) { profs.g2d1_ = *ihis; }
	else if ( gain == 3 ) { profs.g3d1_ = *ihis; }
	else {
	  cerr << "[" << __PRETTY_FUNCTION__ << "]"
	       << " Unexpected gain setting! (" << gain << ")" << endl;
	}
      } else {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected ditigal setting! (" << digital << ")" << endl;
      }
      
    }
    
    // Perform histo analysis
    OptoScanAnalysis::Monitorables mons;
    OptoScanAnalysis::analysis( profs, mons );

  }

  // Some debug
  cout << " Complete list of measured gains for " 
       << monitorables.size() << " modules: ";
  map<uint32_t,OptoScanAnalysis::Monitorables>::const_iterator iter;
  for ( iter = monitorables.begin(); iter != monitorables.end(); iter++ ) {
    cout << iter->second.gain_ << ", ";
  }
  cout << endl;
  
  // Create summary histogram
  SummaryHistogramFactory<OptoScanAnalysis::Monitorables> factory;
  string name = factory.name( histo_, 
			      type_, 
			      view_, 
			      level_ );
  TH1F* summary = new TH1F();
  summary->SetName( name.c_str() );
  factory.generate( histo_,
		    type_,
		    view_, 
		    level_, 
		    monitorables,
		    *summary );

  // Write histo to file
  file_->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineCommissioningClient::vpspScan() {
  cout << DBG << endl;
  
  // Storage for monitorables
  map<uint32_t,VpspScanAnalysis::Monitorables> monitorables;
  
  // Iterate through map
  HistosMap::iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {

    // Iterate through vector of histos
    VpspScanAnalysis::TProfiles profs;
    Histos::iterator ihis = imap->second.begin(); 
    for ( ; ihis != imap->second.end(); ihis++ ) {

      if ( !(*ihis) ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " NULL pointer to histogram!" << endl;
	continue;
      }

      // Retrieve control key
      static SiStripHistoNamingScheme::HistoTitle title;
      title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      
      // Some checks
      if ( title.task_ != sistrip::VPSP_SCAN ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected commissioning task!"
	     << "(" << SiStripHistoNamingScheme::task( title.task_ ) << ")"
	     << endl;
      }

      // Store histo pointers
      if ( title.channel_ >= 32 &&
	   title.channel_ <= 37 ) { 
	if ( title.channel_%2 == 0 ) { 
	  profs.vpsp0_ = *ihis;
	} else if ( title.channel_%2 == 1 ) { 
	  profs.vpsp1_ = *ihis;
	} else {
	  cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected channel setting! (" << title.channel_ << ")" << endl;
	}
      }
      
    }
    
    // Perform histo analysis
    VpspScanAnalysis::Monitorables mons;
    VpspScanAnalysis::analysis( profs, mons );

  }

  // Some debug
  cout << " Complete list of measured VPSP for " 
       << monitorables.size() << " modules: ";
  map<uint32_t,VpspScanAnalysis::Monitorables>::const_iterator iter;
  for ( iter = monitorables.begin(); iter != monitorables.end(); iter++ ) {
    cout << iter->second.vpsp0_ << ", ";
  }
  cout << endl;
  
  // Create summary histogram
  SummaryHistogramFactory<VpspScanAnalysis::Monitorables> factory;
  string name = factory.name( histo_, 
			      type_, 
			      view_, 
			      level_ );
  TH1F* summary = new TH1F();
  summary->SetName( name.c_str() );
  factory.generate( histo_,
		    type_,
		    view_, 
		    level_, 
		    monitorables,
		    *summary );

  // Write histo to file
  file_->Write();
  
}
