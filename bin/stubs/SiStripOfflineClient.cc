#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripOfflineClient.h"
// Histogram analysis
#include "DQM/SiStripCommissioningAnalysis/interface/FedCablingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/ApvTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/FedTimingAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/OptoScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/VpspScanAnalysis.h"
#include "DQM/SiStripCommissioningAnalysis/interface/PedestalsAnalysis.h"
// Summary histograms
#include "DQM/SiStripCommissioningSummary/interface/FedCablingSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/ApvTimingSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/FedTimingSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/OptoScanSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/VpspScanSummaryFactory.h"
#include "DQM/SiStripCommissioningSummary/interface/PedestalsSummaryFactory.h"
// Misc
#include "DQM/SiStripCommissioningSummary/interface/SummaryGenerator.h"
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "TProfile.h"

using namespace std;

// -----------------------------------------------------------------------------
//
SiStripOfflineClient::SiStripOfflineClient( const string& filename,
					    const sistrip::SummaryHisto& histo,
					    const sistrip::SummaryType& type,
					    const string& top_level_dir,
					    const sistrip::Granularity& gran ) 
  : filename_(filename),
    task_(sistrip::UNKNOWN_TASK),
    view_(sistrip::UNKNOWN_VIEW),
    histo_(histo),
    type_(type),
    level_(top_level_dir),
    gran_(gran),
    file_(0),
    run_(0),
    map_()
{

  // Open client file
  file_ = new SiStripCommissioningFile( filename_.c_str() );
  task_ = file_->Task(); 
  view_ = file_->View(); 
  cout << "In file: " << filename_ << endl
       << " commissioning task: " << SiStripHistoNamingScheme::task( task_ ) << endl
       << " logical view:       " << SiStripHistoNamingScheme::view( view_ ) << endl;
  
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
SiStripOfflineClient::~SiStripOfflineClient() {
  if ( file_ ) { delete file_; }
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::analysis() {
  
  // Fill map with commissioning histograms 
  fillHistoMap();

  // Analyse map
  if      ( task_ == sistrip::FED_CABLING ) { fedCabling(); }
  if      ( task_ == sistrip::APV_TIMING )  { apvTiming(); }
  else if ( task_ == sistrip::FED_TIMING )  { fedTiming(); }
  else if ( task_ == sistrip::OPTO_SCAN )   { optoScan(); }
  else if ( task_ == sistrip::VPSP_SCAN )   { vpspScan(); }
  else if ( task_ == sistrip::PEDESTALS )   { pedestals(); }
  else { cerr <<  "[" << __PRETTY_FUNCTION__ << "]"
	      << " Unknown task: " << task_ << endl; }
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::fillHistoMap() {
  
  map< string, vector<TProfile*> > histos;
  
  // Get histograms from client file
  file_->findProfiles( file_->top(), &histos );
  
  // Convert map (to use FEC key as index, rather than directory string)
  map< string, vector<TProfile*> >::iterator iter = histos.begin();
  for ( ; iter != histos.end(); iter++ ) {
    cout << "Directory: " << iter->first << endl;
    uint32_t index = iter->first.find( sistrip::controlView_ );
    string control = iter->first.substr( index );
    SiStripHistoNamingScheme::ControlPath path = SiStripHistoNamingScheme::controlPath( control );
    
    if ( path.fecCrate_ == sistrip::invalid_ ||
	 path.fecSlot_ == sistrip::invalid_ ||
	 path.fecRing_ == sistrip::invalid_ ||
	 path.ccuAddr_ == sistrip::invalid_ ||
	 path.ccuChan_ == sistrip::invalid_ ) { continue; }
	 
    vector<TProfile*>::iterator ihis = iter->second.begin();
    for ( ; ihis != iter->second.end(); ihis++ ) {
      SiStripHistoNamingScheme::HistoTitle title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      uint16_t channel = ( title.granularity_ == sistrip::APV ) ? (title.channel_-32)/2 : title.channel_;
      uint32_t key = SiStripControlKey::key( path.fecCrate_, 
					     path.fecSlot_, 
					     path.fecRing_, 
					     path.ccuAddr_, 
					     path.ccuChan_,
					     channel );
      map_[key].push_back(*ihis);
      cout << "Key: 0x" << hex << setw(8) << setfill('0') << key << dec
	   << "  Histo: " << (*ihis)->GetName() << endl;
    }
  }

  if ( map_.empty() ) {
    cerr << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Zero histograms found!" << endl;
  }
  
}
  
//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::fedCabling() {
  
  map<uint32_t,FedCablingAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    FedCablingAnalysis anal;
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<FedCablingAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::apvTiming() {
  
  map<uint32_t,ApvTimingAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    ApvTimingAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<ApvTimingAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::fedTiming() {
  
  map<uint32_t,FedTimingAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    FedTimingAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<FedTimingAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::optoScan() {
  
  map<uint32_t,OptoScanAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    OptoScanAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<OptoScanAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::vpspScan() {
  
  map<uint32_t,VpspScanAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    VpspScanAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<VpspScanAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::pedestals() {
  
  map<uint32_t,PedestalsAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    PedestalsAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;
    stringstream ss;
    anal.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<PedestalsAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  
  file_->addPath(level_)->cd();
  summary->Write();
  
}

