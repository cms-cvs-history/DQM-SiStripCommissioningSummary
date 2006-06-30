#include "DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h" 

#include <iostream>
#include <sstream>

#include "TString.h"

using namespace std;

//-----------------------------------------------------------------------------

SiStripCommissioningFile::SiStripCommissioningFile(const char* fname, Option_t* option, const char* ftitle, Int_t compress) :
  TFile(fname,option,ftitle,compress),

  task_(sistrip::UNKNOWN_TASK),
  view_(sistrip::UNKNOWN_VIEW),
  top_(gDirectory),
  dqmTop_(0),
  sistripTop_(0),
  dqmFormat_(false)
  
{readDQMFormat();}

//-----------------------------------------------------------------------------

SiStripCommissioningFile::~SiStripCommissioningFile() {;}

//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::setDQMFormat(sistrip::Task task, sistrip::View view) {

   view_ = view;
   task_ = task;

 if (view == sistrip::CONTROL) {
   stringstream ss("");
   ss << dqm << sistrip::dir_ << sistrip::root_ << sistrip::dir_ << sistrip::controlView_;
   top_ = addPath(ss.str());
   dqmTop_ = GetDirectory(dqm.c_str());
   sistripTop_ = dqmTop_->GetDirectory(sistrip::root_.c_str());
   dqmFormat_ = true;

   //TString defining commissioning task
   stringstream task_label;
   task_label << sistrip::commissioningTask_ << sistrip::sep_ << SiStripHistoNamingScheme::task(task_);
   TNamed task_description(task_label.str().c_str(),SiStripHistoNamingScheme::task(task_).c_str());
   sistripTop_->WriteTObject(&task_description);
}

  else {cout << "[CommissioningFile::setDQMFormat]: Currently only implemented for Control View." << endl; return 0;}

 return top_;
}

//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::readDQMFormat() {
  
  //check directory structure and find readout view
  dqmTop_ = GetDirectory(dqm.c_str());
  if (dqmTop_) sistripTop_ = dqmTop_->GetDirectory(sistrip::root_.c_str());
  if (sistripTop_) top_ = sistripTop_->GetDirectory(sistrip::controlView_.c_str());
  if (top_!=gDirectory) view_ = sistrip::CONTROL;

  //does file conform with DQM Format requirements?
  if (dqmTop_ && sistripTop_ && top_) {
    dqmFormat_ = true;}
  
  //Search for commissioning task
  if (sistripTop_) {
  TList* keylist = sistripTop_->GetListOfKeys();
  if (keylist) {
      TObject* obj = keylist->First(); //the object
      if (obj) {
	bool loop = true;
	while (loop) { 
	  if (obj == keylist->Last()) {loop = false;}
	  if (string(obj->GetName()).find(sistrip::commissioningTask_)!=string::npos) {
	    task_ = SiStripHistoNamingScheme::task(string(obj->GetTitle()));
	  }
	  obj = keylist->After(obj);
	}
      }
  }
  } 
  return top_;
}

//-----------------------------------------------------------------------------

bool SiStripCommissioningFile::queryDQMFormat() {
  return dqmFormat_;
}

//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::top() {
  return top_;}

//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::dqmTop() {
  if (!dqmFormat_) {cout << "[SiStripCommissioningFile::dqm]: Error requested dqm directory when not in dqm format." << endl; return 0;}

  return dqmTop_;}


//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::sistripTop() {
  if (!dqmFormat_) {cout << "[SiStripCommissioningFile::dqm]: Error requested dqm directory when not in dqm format." << endl; return 0;}

  return sistripTop_;}

//-----------------------------------------------------------------------------

sistrip::Task& SiStripCommissioningFile::Task() {
  return task_;}

//-----------------------------------------------------------------------------

sistrip::View& SiStripCommissioningFile::View() {
  return view_;}

//-----------------------------------------------------------------------------

void SiStripCommissioningFile::addDevice(unsigned int key) {

  if (view_ == sistrip::CONTROL) {
    if (!dqmFormat_) setDQMFormat(sistrip::UNKNOWN_TASK, sistrip::CONTROL);
    SiStripControlKey::ControlPath control_path = SiStripControlKey::path(key);
    string directory_path = SiStripHistoNamingScheme::controlPath(control_path.fecCrate_,control_path.fecSlot_,control_path.fecRing_,control_path.ccuAddr_,control_path.ccuChan_);
    cd(dqm.c_str());
    addPath(directory_path);
}

 else {cout << "[CommissioningFile::addDevice]: Currently only implemented for Control View." << endl; }

}

//-----------------------------------------------------------------------------

TDirectory* SiStripCommissioningFile::addPath(const string& path) {

  vector<string> directories; directories.reserve(10);

 //fill vector
 string::const_iterator it, previous_dir, latest_dir;
 if (*(path.begin()) == sistrip::dir_) {
   it = previous_dir = latest_dir = path.begin();}
 else {it = previous_dir = latest_dir = path.begin()-1;}

 while (it != path.end()) {
   it++;
   if (*it == sistrip::dir_) {
     previous_dir = latest_dir; 
     latest_dir = it;
     directories.push_back(string(previous_dir+1, latest_dir));
   }
 }

 if (latest_dir != (path.end()-1)) {
   directories.push_back(string(latest_dir+1, path.end()));}
 
 //update file
 TDirectory* child = gDirectory;
 for (vector<string>::const_iterator dir = directories.begin(); dir != directories.end(); dir++) {
   if (!dynamic_cast<TDirectory*>(child->Get(dir->c_str()))) {
   child = child->mkdir(dir->c_str());
   child->cd();}
   else {child->Cd(dir->c_str()); child = gDirectory;}
 }
 return child;
}

//-----------------------------------------------------------------------------

void SiStripCommissioningFile::findProfiles(TDirectory* dir, map< string, vector<TProfile*> >* histos) {

  vector< TDirectory* > dirs;
  dirs.reserve(20000);
  dirs.push_back(dir);

  //loop through all directories and record tprofiles (matching label taskId_) contained within them.

    while ( !dirs.empty() ) { 
     dirContent(dirs[0], &dirs, histos);
      dirs.erase(dirs.begin());
    }
}

//-----------------------------------------------------------------------------


 void SiStripCommissioningFile::dirContent(TDirectory* dir, vector<TDirectory*>* dirs, map< string, vector<TProfile*> >* histos) {

    TList* keylist = dir->GetListOfKeys();

    if (keylist) {
      TObject* obj = keylist->First(); //the object

      if (obj) {
	bool loop = true;
	while (loop) { 
	  if (obj == keylist->Last()) {loop = false;}
 
	  if (dynamic_cast<TDirectory*>(dir->Get(obj->GetName()))) {
	    TDirectory* child = dynamic_cast<TDirectory*>(dir->Get(obj->GetName()));

	    //update record of directories
	    dirs->push_back(child);
	  }

	  if (dynamic_cast<TProfile*>(dir->Get(obj->GetName()))) {
	    TProfile* tprof = dynamic_cast<TProfile*>(dir->Get(obj->GetName()));
	      //update record of tprofiles
	      (*histos)[string(dir->GetPath())].reserve(6);
	      (*histos)[string(dir->GetPath())].push_back(tprof);
	  }
	  obj = keylist->After(obj);
	}
      }
    }
 }

//-----------------------------------------------------------------------------

const string SiStripCommissioningFile::dqm = "DQMData";

//-----------------------------------------------------------------------------


