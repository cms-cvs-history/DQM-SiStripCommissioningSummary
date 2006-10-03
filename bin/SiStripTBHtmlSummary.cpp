#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripTBHtmlSummary.h"
#include <string>
#include <sstream>
#include <iostream> 

using namespace std;

/** 
    @author : M.Wingham
    @brief : Reads in commissioning html file and summarises requested
    monitorable. For OptoScan file the monitorable with for the gain closest
    to the specified target is recorded.
*/

int main( int argc, char* argv[]) {
  
  // Default values for arguments

  std::string cfg_file = "tbHtmlSummary_cfg.dat";
  std::string html_file = "";
  std::string summary_path = "";
  std::string monitorable = "";
  std::string summary_type = "Summary2D";
  float target = 0;
  uint16_t lldChan = 0;
  uint16_t tick = 0;
  
 // Read in arguments to main

  if ( argc > 1 ) { 
    cfg_file  = argv[1]; 
    cout << "Reading config. file: " << cfg_file << endl;
    ifstream in;
    in.open( cfg_file.c_str() );
    if( !in ) {
      stringstream ss;
      ss  << "[" << __PRETTY_FUNCTION__ << "]"
	  << " File could not be opened at '" 
	  << cfg_file << "'";
      cerr << ss.str() << endl;
    } else {
      while ( !in.eof() ) {
	string data;
	getline(in,data); 

	//ignore string after # symbol
	string::size_type pos = 0;
	while (pos != string::npos) {
	  pos = data.find("#",pos);
	  if (pos !=string::npos) {data.erase(pos,(data.size()-pos));}
	}

	//remove whitespace
	pos = 0;
	while (pos != string::npos) {
	  pos = data.find(" ",pos);
	  if (pos !=string::npos) {data.erase(pos,1);}
	}

	//interpret data here....

	pos = data.find("HtmlFile=",0);
	if (pos != string::npos) {
	  html_file = data.substr(pos+9);}

	pos = data.find("SummaryPath=",0);
	if (pos != string::npos) {
	  summary_path = data.substr(pos+12);}

	pos = data.find("Monitorable=",0);
	if (pos != string::npos) {
	  monitorable = data.substr(pos+12);}

	pos = data.find("SummaryType=",0);
	if (pos != string::npos) {
	  summary_type = data.substr(pos+12);}

	pos = data.find("TargetGain=",0);
	if (pos != string::npos) {
	  stringstream ss;
	  ss << data.substr(pos+11);
	  ss >> dec >> target;}

	pos = data.find("LldChan=",0);
	if (pos != string::npos) {
	  lldChan = atoi(data.substr(pos+8).c_str());}

	pos = data.find("Tick=",0);
	if (pos != string::npos) {
	  tick = atoi(data.substr(pos+5).c_str());}
      }
      in.close();
    }
  }

  if ( argc > 2 ) { html_file  = argv[2]; }
  if ( argc > 3 ) { summary_path = argv[3]; }
  if ( argc > 4 ) { monitorable =  argv[4]; }
  if ( argc > 5 ) { summary_type =  argv[5]; }
  if ( argc > 6 ) { stringstream ss; ss << argv[6]; ss >> dec >> target; }
  if ( argc > 7 ) { lldChan = atoi(argv[7]); }
  if ( argc > 8 ) { tick = atoi(argv[8]); }

  std::cout << " SiStripTBHtmlSummary.cpp:" << std::endl
	    << " HtmlFile:                " << html_file << std::endl
	    << " SummaryPath:             " << summary_path << std::endl
	    << " Monitorable:             " << monitorable << std::endl
	    << " Summary Type:            " << summary_type << std::endl
	    << " Target:                  " << target << std::endl
	    << " LldChan:                 " << lldChan << std::endl
	    << " Tick:                 " << tick << std::endl
	    << std::endl;

  ///summarise
  SiStripTBHtmlSummary summary(html_file,summary_path,monitorable,summary_type,target,lldChan,tick);
  if ((!summary.parseHtml()) || (!summary.summary2D())) {
    return 0;}

  //write to file
  if (!summary.write()) {
    return 0;}

  return 1;
}


