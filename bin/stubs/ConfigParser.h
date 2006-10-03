#ifndef ConfigParser_H
#define ConfigParser_H


/** \class ConfigParser
 * *
 *  Parses the xml configuration file for 
 *  the offline client
 * 
 *  $Date: 21/9/2006
 *  $Revision: 1.0
 *  \author Puneeth Kalavase
 */

#include "DQMServices/ClientConfig/interface/DQMParserBase.h"
#include "DQMServices/ClientConfig/interface/ParserFunctions.h"
#include "DQM/SiStripCommon/interface/SiStripEnumeratedTypes.h"
#include "DQM/SiStripCommon/interface/SiStripConstants.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>


class ConfigParser : public DQMParserBase {
  
public:
  /** Default Constructor */
  ConfigParser();
  /** Destructor */
  virtual ~ConfigParser();
  
  /** Struct to hold the information for each requested summary histogram */
  struct SummaryInfo {
  /** Default Constructor */
    SummaryInfo();
  /** Sets the data menmbers back to default */
    void reset();

    sistrip::SummaryHisto histogram;
    sistrip::SummaryType type;
    sistrip::Granularity granularity;
    std::string level;
  };
  
  /** Fill the map with the required tag/names and values */
  void parseXML(std::string);

  /** Fills the vector with client filenames */
  void getFileNames(std::vector<std::string>&);

  /** Returns the SummaryInfo for a given filename. the default settings are returned in the filename is not found in the map. */
  std::vector<SummaryInfo> getSummaryInfo(std::string);

 private:
  //the map which will hold the values
  std::map<std::string, std::vector<SummaryInfo> > settings_map;
  
};

//Default constructor
ConfigParser::SummaryInfo::SummaryInfo() :
  
  histogram(sistrip::UNKNOWN_SUMMARY_HISTO),
     type(sistrip::UNKNOWN_SUMMARY_TYPE),
     granularity(sistrip::UNKNOWN_GRAN),
     level("")
{;}

//Reset data members
void ConfigParser::SummaryInfo::reset() {
  histogram = sistrip::UNKNOWN_SUMMARY_HISTO;
  type = sistrip::UNKNOWN_SUMMARY_TYPE;
  granularity = sistrip::UNKNOWN_GRAN;
  level = "";
}

#endif

  
    
