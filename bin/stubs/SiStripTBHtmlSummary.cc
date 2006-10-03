#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBHtmlSummary.h"

using namespace std;

SiStripTBHtmlSummary::SiStripTBHtmlSummary(string htmlFile, string summaryPath, string monitorable, string summaryType, float targetGain, uint16_t lldChan, uint16_t tick) :
  
  htmlFile_(htmlFile),
  summaryPath_(summaryPath),
  monitorable_(monitorable),
  summaryType_(summaryType),
  targetGain_(targetGain),
  lldChan_(0),
  tick_(0),
  dataMap_(),
  file_(0),
  histo_(0),
  count_(0),
  apvcount_(0),
  gaincount_(0),
  tickcount_(0)
  
{if (monitorable_ == "Gain") {monitorable_ = ">Gain<";}}

//-----------------------------------------------------------------------------

SiStripTBHtmlSummary::~SiStripTBHtmlSummary() {
  if (file_) delete file_;
  if (histo_) delete histo_;}

//-----------------------------------------------------------------------------

bool SiStripTBHtmlSummary::parseHtml() {
  
  ifstream in;
  in.open( htmlFile_.c_str() );
  
  if( !in ) {
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " File could not be opened at '" 
	<< htmlFile_ << "'";
    cerr << ss.str() << endl;
    return false;}
  
  //interpret data here....
  
  //build map of monitorable indexed by bin label string
  stringstream binLabel;
  
  //important strings
  string firstRow = "<td>Module";
  string column = "</td><td>";
  string row = "<td>";
  string bracket = "<";
  string dash = "-";
  string point = ".";
  
  
  while ( !in.eof() ) {
    string data;
    getline(in,data); 
    
    //remove whitespace
    string::size_type pos = 0;
    while (pos != string::npos) {
      pos = data.find(" ",pos);
      if (pos !=string::npos) {data.erase(pos,1);}
    }
    
    //if column headings row
    if (data.find(firstRow,0) != string::npos) {
      
      //1) Find column of monitorable of interest
      string::size_type monitorable_pos = data.find(monitorable_,0);
   
	if (monitorable_pos == string::npos) {   
	  stringstream ss;
	  ss  << "[" << __PRETTY_FUNCTION__ << "]"
	      << " Monitorable not found in file '"
	      << summaryFile() << "'";
	  cerr << ss.str() << endl;
	  return false;}

      if (monitorable_pos != string::npos) {
	while((pos + 10) < monitorable_pos) {
	  pos = data.find(column,pos);
	  count_++;
	  pos++;
	}
      }
      
      //2)Find column of APV number if there
      pos = 0;
      string::size_type apv_pos = data.find("APV",0);
      if (apv_pos != string::npos) {
	while((pos + 10) < apv_pos) {
	  pos = data.find(column,pos);
	  apvcount_++;
	  pos++;
	}
      }
      
      //3)Find column of gain - if there.
      pos = 0;
      string::size_type gain_pos = data.find(">Gain<",0);
      if (gain_pos != string::npos) {
	while((pos + 10) < gain_pos) {
	  pos = data.find(column,pos);
	  gaincount_++;
	  pos++;
	}
      }

     //3)Find column of tick number - if there.
      pos = 0;
      string::size_type tick_pos = data.find(">Tick<",0);
      if (tick_pos != string::npos) {
	while((pos + 10) < tick_pos) {
	  pos = data.find(column,pos);
	  tickcount_++;
	  pos++;
	}
      }
    }
    
    else if (data.find(row,0) != string::npos) {
      
      //Find tick number - if there. Ignore line if it
      //doesnt agree with configurable.

      if (tickcount_) {
	for (uint16_t ii = 0; ii < tickcount_; ii++) {
	  pos = data.find(column,pos)+9;
	}
	string value(data,pos,data.find(bracket,pos)-pos);
	if (atoi(value.c_str()) != tick_) continue;

      }
      //loop string to find control path
      binLabel.str("");
      
      pos = data.find("Fec=")+4;
      string fec(data,pos,data.find(dash,pos) - pos);
      binLabel << fec << point;
      
      pos = data.find("Slot=")+5;
      string slot(data,pos,data.find(dash,pos) - pos);
      binLabel << slot << point;
      
      pos = data.find("Ring=")+5;
      string ring(data,pos,data.find(dash,pos) - pos);
      binLabel << ring << point;
      
      pos = data.find("Ccu=")+4;
      string ccu(data,pos,data.find(dash,pos) - pos);
      binLabel << ccu << point;
      
      pos = data.find("Channel=")+8;
      string chan(data,pos,data.find(column,pos) - pos);
      binLabel << chan;
      
      //Find Apv number - if there
      pos = 0;
      if (apvcount_) {
	for (uint16_t ii = 0; ii < apvcount_; ii++) {
	  pos = data.find(column,pos)+9;
	}
	string value(data,pos,data.find(bracket,pos)-pos);
	uint16_t apvnum = atoi(value.c_str()) -32;
	binLabel << point << apvnum/2;
      }
      
      //If not use configured lld-channel number
      else {binLabel << point << lldChan_;}
      
      //////////////
      
      //find monitorable
      pos=0;
      for (uint16_t ii = 0; ii < count_; ii++) {
	pos = data.find(column,pos)+9;
      }
      stringstream value;
      float val;
      value << string(data,pos,data.find(bracket,pos)-pos);
      value >> dec >> val;
      
      //find gain - if present
      pos=0;
      if (gaincount_) {
	for (uint16_t ii = 0; ii < gaincount_; ii++) {
	  pos = data.find(column,pos)+9;
	}
	stringstream gainvalue;
	float gain;
	gainvalue << string(data,pos,data.find(bracket,pos)-pos);
	gainvalue >> dec >> gain;

	//fill map
	dataMap_[binLabel.str()].push_back(pair<float,float>(val,gain));}
      
      else {
	//fill map
	dataMap_[binLabel.str()].push_back(pair<float,float>(val,9999.));}
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

bool SiStripTBHtmlSummary::summary2D() {

  if (histo_) {  
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " Histogram already defined.";
    cerr << ss.str() << endl;
    return false;}

  //loop Map and fill histogram
  histo_ = new TH2F("","", 1000, 0., static_cast<float>(1000.), 1025, 0., 1025. );
  stringstream title;
  title << "Html_" << monitorable_;
  histo_->SetName(summaryName().c_str());
  histo_->SetTitle(title.str().c_str());
  
  // Set histogram number of bins and min/max
  histo_->GetXaxis()->Set( dataMap_.size(), 0., (Double_t)dataMap_.size() );
  histo_->GetYaxis()->Set( 1025,0.,1025.);
  // Markers (crosses)
  histo_->SetMarkerStyle(2);
  histo_->SetMarkerSize(0.6);
  
  // Select only monitorable corresponding to target gain if required.
  if (gaincount_) {findGain();}

  // Iterate through map, set bin labels and fill histogram. 

  Double_t bin = 0;
  map< string,vector< pair< float,float > > >::const_iterator ibin = dataMap_.begin();
  for ( ; ibin != dataMap_.end(); ibin++ ) {
    
    histo_->GetXaxis()->SetBinLabel( static_cast<Int_t>(bin+1), ibin->first.c_str() );
    
    for (uint16_t ivalue = 0; ivalue < ibin->second.size(); ivalue++) {
      histo_->Fill((Double_t)(bin+.5),(Double_t)(ibin->second[ivalue].first) );}
    bin++;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool SiStripTBHtmlSummary::write() {

  if (!openFile()) {  
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " File could not be opened at '" 
	<< summaryFile() << "'";
    cerr << ss.str() << endl;
    return false;}

 if (!histo_) {  
   stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " There is nothing to write! Fill histogram." ;
    cerr << ss.str() << endl;
    return false;}

 TDirectory* dir = file_->GetDirectory(summaryDir().c_str());

 if (!dir) {  
   stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " Directory could not be opened at '" 
	<< summaryFile() << ":" << summaryDir() <<"'";
    cerr << ss.str() << endl;
    return false;}

 dir->cd();
 histo_->Write();
 return true;}

//-----------------------------------------------------------------------------

bool SiStripTBHtmlSummary::openFile() {

  file_ = new TFile(summaryFile().c_str(),"UPDATE");
  if (!file_) return false;
  return true;
}

//-----------------------------------------------------------------------------

string SiStripTBHtmlSummary::summaryFile() {
  return string(summaryPath_,0,(summaryPath_.find(":/",0)));}

//-----------------------------------------------------------------------------

string SiStripTBHtmlSummary::summaryDir() {

  string::size_type pos = summaryPath_.find(":/",pos);  
  while (summaryPath_.find("/",pos) != string::npos) {
    pos = summaryPath_.find("/",pos);
    pos++;}
  
  return string(summaryPath_,summaryPath_.find(":/",0)+1,pos-summaryPath_.find(":/",0)-1);
}

//-----------------------------------------------------------------------------

string SiStripTBHtmlSummary::summaryName() {
  
  string::size_type pos = summaryPath_.find(":/",pos);  
  while (summaryPath_.find("/",pos) != string::npos) {
    pos = summaryPath_.find("/",pos);
    pos++;}
  return string(summaryPath_,pos,summaryPath_.size()-pos);}

//-----------------------------------------------------------------------------

void SiStripTBHtmlSummary::findGain() {

  map< string,vector< pair< float,float > > >::iterator ibin = dataMap_.begin();
  for ( ; ibin != dataMap_.end(); ibin++ ) {
    uint16_t closestGainIndex = 0;
    for (uint16_t igain = 0; igain < ibin->second.size(); igain++) {
	if (fabs(ibin->second[igain].second - targetGain_) < fabs(ibin->second[closestGainIndex].second - targetGain_)) {closestGainIndex=igain;}
      }

    //delete remaining map members
    uint16_t icount = 0;
    for (vector< pair <float,float> >::iterator it = ibin->second.begin(); it != ibin->second.end(); it++) {
      if (icount != closestGainIndex) {it = ibin->second.erase(it);it--;}
	icount++;
    }
  }
}
