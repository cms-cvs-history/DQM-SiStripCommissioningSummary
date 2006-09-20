
#include <boost/cstdint.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

#include <vector>
#include <map>
#include <utility>

using namespace std;

/** 
    @author : M.Wingham
    @brief : Compares commissioning client summaries and writes difference of
    bins (1-2) with matching labels to the top of file1.
*/

TH1F compare(TH2F* h1, TH2F* h2) {

  Int_t nbins = 0;
  map<string, pair<Float_t, Float_t> > labels; 
  
  //fill map with h1 (mean of points per bin)
  Int_t ibin,ibiny,ipoint;
  Float_t mean;

  for(ibin=0; ibin<h1->GetNbinsX(); ibin++) {
    mean = 0.;
    ipoint = 0;
    for ( ibiny = 0; ibiny < h1->GetNbinsY(); ibiny++) {
      if ((Int_t)h1->GetBinContent(ibin+1,ibiny+1)) {
	mean+=ibiny;ipoint++;}} 
    if (ipoint) {mean = mean/(Float_t)ipoint;}
    string label(h1->GetXaxis()->GetBinLabel(ibin+1));
    labels[label] = pair<Float_t, Float_t>(mean, 9999.); 
  }

  //fill map with h2
  for(ibin=0; ibin<h2->GetNbinsX(); ibin++) {
    mean = 0.;
    ipoint = 0;
    for ( ibiny = 0; ibiny < h2->GetNbinsY(); ibiny++) {
      if ((Int_t)h2->GetBinContent(ibin+1,ibiny+1)) {
	mean+=ibiny;ipoint++;}} 
    if (ipoint){mean = mean/(Float_t)ipoint;}
    string label(h2->GetXaxis()->GetBinLabel(ibin+1));
    if (labels.find(label) != labels.end()) {
      labels[label].second = mean;}
    else {
      labels[label]=pair<Float_t, Float_t>(9999.,mean);} 
  }

  //Check map for matching bin labels
  map<string, pair<Float_t, Float_t> >::const_iterator iter=labels.begin();
  for (;iter != labels.end();iter++) {
    if ((iter->second.first > 9000.) || (iter->second.second > 9000.)) {
      cout << "Warning: Histogram bins do not match. Unpaired bin label: " << string(iter->first) << endl;
    }
    else {nbins++;}
  }

  //fill comparison histogram
    
  string s1=h1->GetTitle();
  string s2=h2->GetTitle();
  string s="Validation_" + s1 +"_" + s2 ;
  const char* c=s.c_str();
  TH1F h("h", c , nbins, 0., (Float_t)nbins);

  //fill histogram with the difference
  iter=labels.begin();
  ibin=0;
  Float_t diff;
  for (;iter != labels.end();iter++) {
    if (((*iter).second.first < 9000.) && ((*iter).second.second < 9000.)) {
      diff=((*iter).second).first-((*iter).second).second;
      h.GetXaxis()->SetBinLabel((ibin+1), (*iter).first.c_str());
      h.SetBinContent((ibin+1), diff);
      ibin++;}
  }
  return h;
}

  int main( int argc, char* argv[]) {
  
  // Default values for arguments

  string cfg_file = "compareSummary_cfg.dat";
  string summary_filename1 = "";
  string summary_path1 = "";
  string summary_name1 = "";
  string summary_filename2 = "";
  string summary_path2 = "";
  string summary_name2 = "";

  
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

	//interpret data here....

	string::size_type pos = data.find("Summary1=",0);
	if (pos != string::npos) {
	  pos+=9;
	  summary_filename1 = string(data,pos,(data.find(":/",pos)-pos));
	  pos = data.find(":/",pos);
	  while (data.find("/",pos) != string::npos) {
	    pos = data.find("/",pos);
	    pos++;}
	  summary_path1 = string(data,data.find(":/",0)+1,pos-data.find(":/",0)-1);
	  summary_name1 = string(data,pos,data.size()-pos);}
	
	pos = data.find("Summary2=",0);
	if (pos != string::npos) {
	  pos+=9;
	  summary_filename2 = string(data,pos,(data.find(":/",pos)-pos));
	  pos = data.find(":/",pos);
	  while (data.find("/",pos) != string::npos) {
	    pos = data.find("/",pos);
	    pos++;}
	  summary_path2 = string(data,data.find(":/",0)+1,pos-data.find(":/",0)-1);
	  summary_name2 = string(data,pos,data.size()-pos);}
      }
      in.close();
    }
  }

  if ( argc > 2 ) { 
    string data(argv[2]);
    string::size_type pos = 0;
    summary_filename1 = string(data,pos,(data.find(":/",pos)-pos));
    pos = data.find(":/",pos);
    while (data.find("/",pos) != string::npos) {
      pos = data.find("/",pos);
      pos++;}
    summary_path1 = string(data,data.find(":/",0)+1,pos-data.find(":/",0)-1);
    summary_name1 = string(data,pos,data.size()-pos);}

  if ( argc > 3 ) { 
    string data(argv[3]);
    string::size_type pos = 0;
    summary_filename2 = string(data,pos,(data.find(":/",pos)-pos));
    pos = data.find(":/",pos);
    while (data.find("/",pos) != string::npos) {
      pos = data.find("/",pos);
      pos++;}
    summary_path2 = string(data,data.find(":/",0)+1,pos-data.find(":/",0)-1);
    summary_name2 = string(data,pos,data.size()-pos);}


  
  std::cout << " SiStripCompareSummaryHistos.cpp: " << std::endl
	    << " Summary1:                        " << summary_filename1 << ":" << summary_path1 << summary_name1 << std::endl
	    << " Summary2:                        " << summary_filename2 << ":" << summary_path2 << summary_name2 << std::endl;

  //open files

  TFile* summary_file1 = new TFile(summary_filename1.c_str(),"UPDATE");  
  TFile* summary_file2 = new TFile(summary_filename2.c_str(),"READ");

  if(!summary_file1) {
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " File could not be opened at '" 
	<< summary_filename1 << "'";
    cerr << ss.str() << endl;
    return 0;
  } 

  if(!summary_file2) {
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " File could not be opened at '" 
	<< summary_filename2 << "'";
    cerr << ss.str() << endl;
    return 0;
  } 

  //extract histos

  TH2F* summary1 = dynamic_cast<TH2F*>(summary_file1->GetDirectory(summary_path1.c_str())->Get(summary_name1.c_str()));

  TH2F* summary2 = dynamic_cast<TH2F*>(summary_file2->GetDirectory(summary_path2.c_str())->Get(summary_name2.c_str()));

 if(!summary1) {
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " TH1F could not be opened at '" 
	<< summary_filename1 << ":" << summary_path1;
    cerr << ss.str() << endl;
    return 0;
  } 

  if(!summary2) {
    stringstream ss;
    ss  << "[" << __PRETTY_FUNCTION__ << "]"
	<< " TH1F could not be opened at '" 
	<< summary_filename2 <<  ":" << summary_path2;
    cerr << ss.str() << endl;
    return 0;
  } 

  //compare
  summary_file1->cd();
  compare(summary1,summary2).Write();

  //clean-up
  delete summary_file1;
  delete summary_file2;

  return 0;
}
    
