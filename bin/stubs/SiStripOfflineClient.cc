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
#include "DataFormats/SiStripCommon/interface/SiStripHistoNamingScheme.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
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
    path_(""),
    gran_(gran),
    file_(0),
    run_(0),
    map_()
{

  // Open client file
  file_ = new SiStripCommissioningFile( filename_.c_str() );
  task_ = file_->Task(); 
  view_ = file_->View(); 
  path_ = "DQMData/SiStrip/" /*+ SiStripHistoNamingScheme::view( view_ ) + "/"*/ + level_;
  cout << "In file: " << filename_ << endl
       << " commissioning task: " << SiStripHistoNamingScheme::task( task_ ) << endl
       << " logical view:       " << SiStripHistoNamingScheme::view( view_ ) << endl
       << " directory path:     " << path_ << endl;
  
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
    //cout << "Directory: " << iter->first << endl;
    uint32_t index = iter->first.find( sistrip::controlView_ );
    string control = iter->first.substr( index );
    SiStripFecKey::Path path = SiStripHistoNamingScheme::controlPath( control );
    
    if ( path.fecCrate_ == sistrip::invalid_ ||
	 path.fecSlot_ == sistrip::invalid_ ||
	 path.fecRing_ == sistrip::invalid_ ||
	 path.ccuAddr_ == sistrip::invalid_ ||
	 path.ccuChan_ == sistrip::invalid_ ) { continue; }
	 
    vector<TProfile*>::iterator ihis = iter->second.begin();
    for ( ; ihis != iter->second.end(); ihis++ ) {
      HistoTitle title = SiStripHistoNamingScheme::histoTitle( (*ihis)->GetName() );
      uint16_t channel = ( (title.granularity_ == sistrip::APV) && (title.channel_ >= 32) ) ? (title.channel_-32)/2 : title.channel_;

      uint32_t key = SiStripFecKey::key( path.fecCrate_, 
					 path.fecSlot_, 
					 path.fecRing_, 
					 path.ccuAddr_, 
					 path.ccuChan_,
					 channel );
      map_[key].push_back(*ihis);
      //cout << "Key: 0x" << hex << setw(8) << setfill('0') << key << dec
      //<< "  Histo: " << (*ihis)->GetName() << endl;
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
    FedCablingAnalysis anal( imap->first );
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
  
  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }

}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::apvTiming() {
  
  // Reset minimum / maximum delays
  float time_min =  1. * sistrip::invalid_;
  float time_max = -1. * sistrip::invalid_;
  uint32_t device_min = sistrip::invalid_;
  uint32_t device_max = sistrip::invalid_;

  map<uint32_t,ApvTimingAnalysis> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {

    // Perform histo analysis
    ApvTimingAnalysis anal( imap->first );
    anal.analysis( imap->second );
    monitorables[imap->first] = anal;

    // Check tick height is valid
    if ( anal.height() < 100. ) { 
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Tick mark height too small: " << anal.height() << endl;
      continue; 
    }
    
    // Check time of rising edge
    if ( anal.time() > sistrip::maximum_ ) { continue; }
    
    // Find maximum time
    if ( anal.time() > time_max ) { 
      time_max = anal.time(); 
      device_max = imap->first;
    }
    
    // Find minimum time
    if ( anal.time() < time_min ) { 
      time_min = anal.time(); 
      device_min = imap->first;
    }

  }
  
  // Adjust maximum (and minimum) delay(s) to find optimum sampling point(s)
  if ( time_max > sistrip::maximum_ ||
       time_max < -1.*sistrip::maximum_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unable to set maximum time! Found unexpected value: "
	 << time_max << endl;
  } else {
    SiStripFecKey::Path max = SiStripFecKey::path( device_max );
    cout << " Device (FEC/slot/ring/CCU/module/channel) " 
	 << max.fecCrate_ << "/" 
	 << max.fecSlot_ << "/" 
	 << max.fecRing_ << "/" 
	 << max.ccuAddr_ << "/"
	 << max.ccuChan_ << "/"
	 << " has maximum delay (rising edge) [ns]:" << time_max << endl;
    
    SiStripFecKey::Path min = SiStripFecKey::path( device_min );
    cout << " Device (FEC/slot/ring/CCU/module/channel): " 
	 << min.fecCrate_ << "/" 
	 << min.fecSlot_ << "/" 
	 << min.fecRing_ << "/" 
	 << min.ccuAddr_ << "/"
	 << min.ccuChan_ << "/"
	 << " has minimum delay (rising edge) [ns]:" << time_min << endl;
  }
  
  // Set maximum time for all analysis objects
  map<uint32_t,ApvTimingAnalysis>::iterator ianal = monitorables.begin();
  for ( ; ianal != monitorables.end(); ianal++ ) { 
    ianal->second.maxTime( time_max ); 
    stringstream ss;
    ianal->second.print( ss ); 
    cout << ss.str() << endl;
  }
  
  SummaryHistogramFactory<ApvTimingAnalysis> factory;
  factory.init( histo_, type_, view_, level_, gran_ );
  uint32_t xbins = factory.extract( monitorables );
  TH1* summary = SummaryGenerator::histogram( type_, xbins );
  factory.fill( *summary );
  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }
  
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
  
  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }

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
  
  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }
  
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
  
  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }
  
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

  if ( file_ && summary ) { 
    file_->addPath(path_)->cd();
    summary->Write();
    delete summary;
  }

}

