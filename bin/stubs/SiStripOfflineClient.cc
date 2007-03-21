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
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
#include "DataFormats/SiStripCommon/interface/SiStripHistoTitle.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "TProfile.h"

using namespace std;
using namespace sistrip;

// -----------------------------------------------------------------------------
//
SiStripOfflineClient::SiStripOfflineClient( const string& root_file,
					    const string& xml_file )
  : rootFile_(root_file),
    xmlFile_(xml_file),
    file_(0),
    runType_(sistrip::UNKNOWN_RUN_TYPE),
    view_(sistrip::UNKNOWN_VIEW),
    run_(0),
    map_(),
    plots_()
{
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
  edm::LogError(mlDqmClient_)
    << "[SiStripOfflineClient::" << __func__ << "]"
    << " Performing analysis of .root histogram file \"" 
    << rootFile_ << "\" using XML file \"" 
    << xmlFile_ << "\"...";
  
  // Check if .root file can be opened
  ifstream in;
  in.open( rootFile_.c_str() );
  if( !in ) {
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " The .root file \"" << rootFile_
      << "\" could not be opened!";
    return;
  } else { in.close(); }

  // Check if .xml file can be opened
  ifstream in1;
  in1.open( xmlFile_.c_str() );
  if( !in1 ) {
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " The .xml file \"" << xmlFile_
      << "\" could not be opened!";
    return;
  } else { in1.close(); }
  
  // Open client file
  file_ = new SiStripCommissioningFile( rootFile_.c_str() );
  runType_ = file_->runType(); 
  view_ = file_->View(); 

  edm::LogVerbatim(mlDqmClient_)
    << "[SiStripOfflineClient::" << __func__ << "] Some information..." << "\n"
    << " Opened .root file:  " << rootFile_ << "\n"
    << " Commissioning runType: " << SiStripEnumsAndStrings::runType( runType_ ) << "\n"
    << " Logical view:       " << SiStripEnumsAndStrings::view( view_ ) << "\n"
    << " Using XML file:     " << xmlFile_;

  // Some checks
  if ( !file_->queryDQMFormat() ) { 
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Error when reading file: " 
      << rootFile_;
    return;
  }
  
  // Check runType
  if ( runType_ == sistrip::UNKNOWN_RUN_TYPE ) { 
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Unknown commissioning runType: " 
      << SiStripEnumsAndStrings::runType( runType_ );
    return;
  }

  // Check view
  if ( view_ == sistrip::UNKNOWN_VIEW ) {
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Unknown view: " 
      << SiStripEnumsAndStrings::view( view_ );
    return;
  }

  // Set run number based on .root filename
  setRunNumber();

  // Parse xml file
  ConfigParser cfg;
  cfg.parseXML(xmlFile_);
  plots_ = cfg.summaryPlots(runType_);
  
  // Fill map with commissioning histograms 
  fillHistoMap();
  
  // Analyse map
  if ( runType_ == sistrip::FED_CABLING ) { fedCabling(); }
  else if ( runType_ == sistrip::APV_TIMING ) { apvTiming(); }
  else if ( runType_ == sistrip::FED_TIMING ) { fedTiming(); }
  else if ( runType_ == sistrip::OPTO_SCAN ) { optoScan(); }
  else if ( runType_ == sistrip::VPSP_SCAN ) { vpspScan(); }
  else if ( runType_ == sistrip::PEDESTALS ) { pedestals(); }
  else { 
    edm::LogError(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Unable to analyze this runType: " << runType_ << "\n"; 
  }

  edm::LogError(mlDqmClient_)
    << "[SiStripOfflineClient::" << __func__ << "]"
    << " Finished analyzing .root file...";
    
}

//-----------------------------------------------------------------------------
//
void SiStripOfflineClient::fillHistoMap() {
  
  map< string, vector<TH1*> > histos;
  
  // Get histograms from client file
  file_->findHistos( file_->top(), &histos );

  // Convert map (to use FEC key as index, rather than directory string)
  map< string, vector<TH1*> >::iterator iter = histos.begin();
  for ( ; iter != histos.end(); iter++ ) {
    uint32_t index = iter->first.find( sistrip::controlView_ );
    string control = iter->first.substr( index );
    SiStripFecKey path( control );
    
    if ( path.fecCrate() == sistrip::invalid_ ||
	 path.fecSlot() == sistrip::invalid_ ||
	 path.fecRing() == sistrip::invalid_ ||
	 path.ccuAddr() == sistrip::invalid_ ||
	 path.ccuChan() == sistrip::invalid_ ) { continue; }
	 
    vector<TH1*>::iterator ihis = iter->second.begin();
    for ( ; ihis != iter->second.end(); ihis++ ) {
      
      SiStripHistoTitle title( (*ihis)->GetName() );
      
      uint16_t channel = sistrip::invalid_;
      if ( title.granularity() == sistrip::APV ) {
	channel = (title.channel()-32)/2;
      } else if ( title.granularity() == sistrip::LLD_CHAN ) {
	channel = title.channel();
      } else {
	edm::LogWarning(mlDqmClient_)
	  << "[SiStripOfflineClient::" << __func__ << "]"
	  << " Unexpected histogram granularity: "
	  << SiStripEnumsAndStrings::granularity( title.granularity() );
      }
      uint32_t key = SiStripFecKey( path.fecCrate(), 
				    path.fecSlot(), 
				    path.fecRing(), 
				    path.ccuAddr(), 
				    path.ccuChan(),
				    channel ).key();

      map_[key].push_back(*ihis);
      
    }
  }

  if ( map_.empty() ) {
    edm::LogWarning(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Found zero histograms!";
  } else {
    edm::LogVerbatim(mlDqmClient_)
      << "[SiStripOfflineClient::" << __func__ << "]"
      << " Found " << map_.size() << " histograms!";
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
    cout << ss.str() << "\n";
  }

  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {

    SummaryHistogramFactory<FedCablingAnalysis> factory;
    factory.init( iplot->mon_, 
		  iplot->pres_, 
		  iplot->view_, 
		  iplot->level_, 
		  iplot->gran_ );
    uint32_t xbins = factory.extract( monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

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

  map<uint32_t,ApvTimingAnalysis*> monitorables;
  HistosMap::const_iterator imap = map_.begin(); 
  for ( ; imap != map_.end(); imap++ ) {
    
    // Perform histo analysis
    ApvTimingAnalysis* anal = new ApvTimingAnalysis( imap->first );
    anal->analysis( imap->second );
    monitorables[imap->first] = anal;

    // Check tick height is valid
    if ( anal->height() < 100. ) { continue; }
    
    // Check time of rising edge
    if ( anal->time() > sistrip::maximum_ ) { continue; }
    
    // Find maximum time
    if ( anal->time() > time_max ) { 
      time_max = anal->time(); 
      device_max = imap->first;
    }
    
    // Find minimum time
    if ( anal->time() < time_min ) { 
      time_min = anal->time(); 
      device_min = imap->first;
    }

  }
  
  // Adjust maximum (and minimum) delay(s) to find optimum sampling point(s)
  if ( time_max > sistrip::maximum_ ||
       time_max < -1.*sistrip::maximum_ ) { 
    cerr << "[SiStripOfflineClient::" << __func__ << "]"
	 << " Unable to set maximum time! Found unexpected value: "
	 << time_max << "\n";
  } else {
    SiStripFecKey max( device_max );
    cout << " Device (FEC/slot/ring/CCU/module/channel) " 
	 << max.fecCrate() << "/" 
	 << max.fecSlot() << "/" 
	 << max.fecRing() << "/" 
	 << max.ccuAddr() << "/"
	 << max.ccuChan() << "/"
	 << " has maximum delay (rising edge) [ns]:" << time_max << "\n";
    
    SiStripFecKey min( device_min );
    cout << " Device (FEC/slot/ring/CCU/module/channel): " 
	 << min.fecCrate() << "/" 
	 << min.fecSlot() << "/" 
	 << min.fecRing() << "/" 
	 << min.ccuAddr() << "/"
	 << min.ccuChan() << "/"
	 << " has minimum delay (rising edge) [ns]:" << time_min << "\n";
  }
  
  // Set maximum time for all analysis objects
  map<uint32_t,ApvTimingAnalysis*>::iterator ianal = monitorables.begin();
  for ( ; ianal != monitorables.end(); ianal++ ) { 
    ianal->second->maxTime( time_max ); 
    stringstream ss;
    ianal->second->print( ss ); 
    cout << ss.str() << "\n";
  }

  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {
    
    SummaryPlotFactory<ApvTimingAnalysis*> factory;
    uint32_t xbins = factory.init( iplot->mon_, 
				   iplot->pres_, 
				   iplot->view_, 
				   iplot->level_, 
				   iplot->gran_,
				   monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

  }
  
  map<uint32_t,ApvTimingAnalysis*>::iterator iter = monitorables.begin();
  for ( ; iter != monitorables.end(); iter++ ) {
    if ( iter->second ) { delete iter->second; }
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
    cout << ss.str() << "\n";
  }
  
  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {
    
    SummaryHistogramFactory<FedTimingAnalysis> factory;
    factory.init( iplot->mon_, 
		  iplot->pres_, 
		  iplot->view_, 
		  iplot->level_, 
		  iplot->gran_ );
    uint32_t xbins = factory.extract( monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

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
    cout << ss.str() << "\n";
  }
  
  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {

    SummaryHistogramFactory<OptoScanAnalysis> factory;
    factory.init( iplot->mon_, 
		  iplot->pres_, 
		  iplot->view_, 
		  iplot->level_, 
		  iplot->gran_ );
    uint32_t xbins = factory.extract( monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

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
    cout << ss.str() << "\n";
  }
  
  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {

    SummaryHistogramFactory<VpspScanAnalysis> factory;
    factory.init( iplot->mon_, 
		  iplot->pres_, 
		  iplot->view_, 
		  iplot->level_, 
		  iplot->gran_ );
    uint32_t xbins = factory.extract( monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

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
    cout << ss.str() << "\n";
  }
  
  // Iterate though plots and create
  vector<ConfigParser::SummaryPlot>::const_iterator iplot = plots_.begin();
  for ( ; iplot != plots_.end(); iplot++ ) {

    SummaryHistogramFactory<PedestalsAnalysis> factory;
    factory.init( iplot->mon_, 
		  iplot->pres_, 
		  iplot->view_, 
		  iplot->level_, 
		  iplot->gran_ );
    uint32_t xbins = factory.extract( monitorables );
    TH1* summary = SummaryGenerator::histogram( iplot->pres_, xbins );
    factory.fill( *summary );
    
    if ( file_ && summary ) { 
      file_->sistripTop()->cd();
      file_->addPath(iplot->level_)->cd();
      summary->Write();
      delete summary;
    }

  }

}

