#ifndef DQM_SiStripCommissioningSummary_SiStripCommissioningFile_h
#define DQM_SiStripCommissioningSummary_SiStripCommissioningFile_h

//common
#include "DQM/SiStripCommon/interface/SiStripHistoNamingScheme.h"

//root
#include "TFile.h"
#include "TProfile.h"
#include "TDirectory.h"

#include <map>
#include <vector>
#include <string>

/** 
    @file : DQM/SiStripCommissioningSummary/bin/stubs/SiStripCommissioningFile.h
    @class : SiStripCommissioningFile
    @author : M.Wingham

    @brief : Inherits from TFile with added functionality to easily build and navigate commissioning files. 
*/

class SiStripCommissioningFile : public TFile {

 public:

  /** Constructor */
  SiStripCommissioningFile( const char* fname, 
			    Option_t* option = "UPDATE", 
			    const char* ftitle = "", 
			    Int_t compress = 1 );
  
  /** Destructor */
  virtual ~SiStripCommissioningFile();

  /** Formats the commissioning file with the correct "top-level" directory structure. Inserts string defining commissioning task in sistrip::root_ directory */
  TDirectory* setDQMFormat( sistrip::Task, sistrip::View );

  /** Checks file complies with DQM format requirements. If so, updates record directory "top-level" directory structure and of readout view and commissioning task. */
  TDirectory* readDQMFormat();

  /** Checks to see if the file complies with DQM format requirements. */
  bool queryDQMFormat();

  /** Returns the "top" directory (describing the readout view) */
  TDirectory* top();

  /** Returns the dqm directory */
  TDirectory* dqmTop();

  /** Returns the sistrip::root_ directory */
  TDirectory* sistripTop();

  /** Get Method */
  sistrip::Task& Task();

  /** Get Method */
  sistrip::View& View();

  /** Adds the directory paths for the device of given key. Must use dqmFormat() before this method. */
  void addDevice(unsigned int key);

  /** Adds a path to the file. Any directories within the path that already exist are not recreated.*/
  TDirectory* addPath(const std::string&);

  /** Finds TProfiles, iterating through sub-directories. Fills a map, indexed by the histogram path. */
  void findProfiles(TDirectory*, std::map< std::string, std::vector<TProfile*> >*);

  /** Finds TProfiles and TDirectories conatined within the directory. Updates the map with the TProfiles found, indexed by the path.*/
  void dirContent(TDirectory*, std::vector<TDirectory*>*, std::map< std::string, std::vector<TProfile*> >*);

  /** */
  static const std::string dqm;
  
 private:
  /** Commissioning task */
  sistrip::Task task_;

  /** Readout view */
  sistrip::View view_;

  /** Readout view directory */
  TDirectory* top_;

  /** dqm directory */
  TDirectory* dqmTop_;

  /** sistrip::root_ directory */
  TDirectory* sistripTop_;

  /** True if dqmFormat() operation has been performed */
  bool dqmFormat_;

};

#endif // DQM_SiStripCommissioningSummary_SiStripCommissioningFile_h
