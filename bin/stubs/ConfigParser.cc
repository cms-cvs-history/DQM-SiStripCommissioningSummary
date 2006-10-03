/** \file
 *
 *  Implementation of ConfigParser
 *
 *  $Date: 21/9/2006 $
 *  Revision: 1.0
 *  \author Puneeth Kalavase
 */

#include "DQM/SiStripCommissioningSummary/bin/stubs/ConfigParser.h"
#include <stdexcept>
#include <map>
#include <vector>

using namespace std;

ConfigParser::ConfigParser() {
  settings_map.clear();

  try {
    XMLPlatformUtils::Initialize();
  }
  
  catch (const XMLException &f) {
    throw(std::runtime_error("Standard pool exception : Fatal Error on pool::TrivialFileCatalog"));
  }
}

ConfigParser::~ConfigParser() {;}


void ConfigParser::getFileNames(std::vector<std::string>& vect_fnames) {
  
  if(settings_map.empty())
    std::cout << "You have not called the parseXML function, or your XML file is erronious" << std::endl;
  
  std::map<std::string, std::vector<SummaryInfo> >::iterator iter;
  for(iter=settings_map.begin(); iter!=settings_map.end(); iter++) {
    vect_fnames.push_back((*iter).first);
  }  
}


std::vector<ConfigParser::SummaryInfo> ConfigParser::getSummaryInfo(std::string fname) {
  
  if(settings_map.empty())
    std::cout << "You have not called the parseXML function, or your XML file is erronious" << std::endl;
  if(settings_map.find(fname)!=settings_map.end())
    return settings_map[fname];
  else {
    std::vector<SummaryInfo> blank;
    return blank;}
}


void ConfigParser::parseXML(std::string f) {
  
  
  settings_map.clear();
  std::vector<ConfigParser::SummaryInfo>  v1;  //vector of structs of options for each file
  getDocument(f);
  DOMNode *root = doc->getDocumentElement();
  std::string fname;
  SummaryInfo options;  
  DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT, NULL, true);
  bool BEGINNING=true;
  bool FIRSTHISTO=true;
  DOMElement *elem = dynamic_cast <DOMElement *>(iter->nextNode());
  unsigned int i=0;
  while(elem!=NULL) {
    i++;
    if(std::strcmp((qtxml::_toString(elem->getTagName())).c_str(), "ClientFile")==0) {
      fname=qtxml::_toString(elem->getAttribute(qtxml::_toDOMS("name")));}

    if(std::strcmp( (qtxml::_toString(elem->getTagName())).c_str(), "SummaryHisto")==0) {
    
      if(!FIRSTHISTO) {
      v1.push_back(options);
      options.reset();}
      else {
	FIRSTHISTO=false;}

      string summaryhisto =  qtxml::_toString(elem->getAttribute(qtxml::_toDOMS("histo")));
      if (summaryhisto == sistrip::undefinedSummaryHisto_) {
	options.histogram = sistrip::UNDEFINED_SUMMARY_HISTO;}
      else if (summaryhisto == sistrip::undefinedSummaryHisto_) {
	options.histogram  = sistrip::UNDEFINED_SUMMARY_HISTO;}
      else if (summaryhisto == sistrip::apvTimingTime_) {
	options.histogram = sistrip::APV_TIMING_TIME;}
      else if (summaryhisto == sistrip::apvTimingMax_) {
	options.histogram = sistrip::APV_TIMING_MAX_TIME;}
      else if (summaryhisto == sistrip::apvTimingDelay_) {
	options.histogram = sistrip::APV_TIMING_DELAY;}
      else if (summaryhisto == sistrip::apvTimingError_) {
	options.histogram = sistrip::APV_TIMING_ERROR;}
      else if (summaryhisto == sistrip::apvTimingBase_) {
	options.histogram = sistrip::APV_TIMING_BASE;}
      else if (summaryhisto == sistrip::apvTimingPeak_) {
	options.histogram = sistrip::APV_TIMING_PEAK;}
      else if (summaryhisto == sistrip::apvTimingHeight_) {
	options.histogram = sistrip::APV_TIMING_HEIGHT;}
      else if (summaryhisto == sistrip::fedTimingTime_) {
	options.histogram = sistrip::FED_TIMING_TIME;}
      else if (summaryhisto == sistrip::fedTimingMax_) {
	options.histogram = sistrip::FED_TIMING_MAX_TIME;}
      else if (summaryhisto == sistrip::fedTimingDelay_) {
	options.histogram = sistrip::FED_TIMING_DELAY;}
      else if (summaryhisto == sistrip::fedTimingError_) {
	options.histogram = sistrip::FED_TIMING_ERROR;}
      else if (summaryhisto == sistrip::fedTimingBase_) {
	options.histogram = sistrip::FED_TIMING_BASE;}
      else if (summaryhisto == sistrip::fedTimingPeak_) {
	options.histogram = sistrip::FED_TIMING_PEAK;}
      else if (summaryhisto == sistrip::fedTimingHeight_) {
	options.histogram = sistrip::FED_TIMING_HEIGHT;}
      else if (summaryhisto == sistrip::optoScanLldBias_) {
	options.histogram = sistrip::OPTO_SCAN_LLD_BIAS_SETTING;}
      else if (summaryhisto == sistrip::optoScanLldGain_) {
	options.histogram = sistrip::OPTO_SCAN_LLD_GAIN_SETTING;}
      else if (summaryhisto == sistrip::optoScanMeasGain_) {
	options.histogram = sistrip::OPTO_SCAN_MEASURED_GAIN;}
      else if (summaryhisto == sistrip::optoScanZeroLight_) {
	options.histogram = sistrip::OPTO_SCAN_ZERO_LIGHT_LEVEL;}
      else if (summaryhisto == sistrip::optoScanLinkNoise_) {
	options.histogram = sistrip::OPTO_SCAN_LINK_NOISE;}
      else if (summaryhisto == sistrip::optoScanBaseLiftOff_) {
	options.histogram = sistrip::OPTO_SCAN_BASELINE_LIFT_OFF;}
      else if (summaryhisto == sistrip::optoScanLaserThresh_) {
	options.histogram = sistrip::OPTO_SCAN_LASER_THRESHOLD;}
      else if (summaryhisto == sistrip::optoScanTickHeight_) {
	options.histogram = sistrip::OPTO_SCAN_TICK_HEIGHT;}
      else if (summaryhisto == sistrip::vpspScanBothApvs_) {
	options.histogram = sistrip::VPSP_SCAN_BOTH_APVS;}
      else if (summaryhisto == sistrip::vpspScanApv0_) {
	options.histogram = sistrip::VPSP_SCAN_APV0;}
      else if (summaryhisto == sistrip::vpspScanApv1_) {
	options.histogram = sistrip::VPSP_SCAN_APV1;}
      else if (summaryhisto == sistrip::pedestalsAllStrips_) {
	options.histogram = sistrip::PEDESTALS_ALL_STRIPS;}
      else if (summaryhisto == sistrip::pedestalsMean_) {
	options.histogram = sistrip::PEDESTALS_MEAN;}
      else if (summaryhisto == sistrip::pedestalsSpread_) {
	options.histogram = sistrip::PEDESTALS_SPREAD;}
      else if (summaryhisto == sistrip::pedestalsMax_) {
	options.histogram = sistrip::PEDESTALS_MAX;}
      else if (summaryhisto == sistrip::pedestalsMin_) {
	options.histogram = sistrip::PEDESTALS_MIN;}
      else if (summaryhisto == sistrip::noiseAllStrips_) {
	options.histogram = sistrip::NOISE_ALL_STRIPS;}
      else if (summaryhisto == sistrip::noiseMean_) {
	options.histogram = sistrip::NOISE_MEAN;}
      else if (summaryhisto == sistrip::noiseSpread_) {
	options.histogram = sistrip::NOISE_SPREAD;}
      else if (summaryhisto == sistrip::noiseMax_) {
	options.histogram = sistrip::NOISE_MAX;}
      else if (summaryhisto == sistrip::noiseMin_) {
	options.histogram = sistrip::NOISE_MIN;}
      else if (summaryhisto == sistrip::numOfDead_) {
	options.histogram = sistrip::NUM_OF_DEAD;}
      else if (summaryhisto == sistrip::numOfNoisy_) {
	options.histogram = sistrip::NUM_OF_NOISY;}}
    
    if(std::strcmp((qtxml::_toString(elem->getTagName())).c_str(), "SummaryLevel")==0) {
      options.level =  qtxml::_toString(elem->getAttribute(qtxml::_toDOMS("level")));}
 
    if(std::strcmp((qtxml::_toString(elem->getTagName())).c_str(), "SummaryType")==0) {
      string summarytype =  qtxml::_toString(elem->getAttribute(qtxml::_toDOMS("type")));
      
      if (summarytype == sistrip::undefinedSummaryType_) {
	options.type = sistrip::UNDEFINED_SUMMARY_TYPE;}
      else if (summarytype == sistrip::summaryDistr_) {
	options.type = sistrip::SUMMARY_DISTR;}
      else if (summarytype == sistrip::summary1D_) {
	options.type = sistrip::SUMMARY_1D;}
      else if (summarytype == sistrip::summary2D_) {
	options.type = sistrip::SUMMARY_2D;}
      else if (summarytype == sistrip::summaryProf_) {
	options.type = sistrip::SUMMARY_PROF;}}
 
    if(std::strcmp((qtxml::_toString(elem->getTagName())).c_str(), "SummaryGran")==0) {
      string granularity =  qtxml::_toString(elem->getAttribute(qtxml::_toDOMS("gran")));

      if (granularity == sistrip::fecCrate_) {
	options.granularity = sistrip::FEC_CRATE;}
      else if (granularity == sistrip::fecSlot_) {
	options.granularity = sistrip::FEC_SLOT;}
      else if (granularity == sistrip::fecRing_) {
	options.granularity = sistrip::FEC_RING;}
      else if (granularity == sistrip::ccuAddr_) {
	options.granularity = sistrip::CCU_ADDR;}
      else if (granularity == sistrip::ccuChan_) {
	options.granularity = sistrip::CCU_CHAN;}
      else if (granularity == sistrip::lldChan_) {
	options.granularity = sistrip::LLD_CHAN;}
      else if (granularity == sistrip::apv_) {
	options.granularity = sistrip::APV;}} 

    elem=dynamic_cast <DOMElement *>(iter->nextNode());
    
    if(elem!=NULL) {  //for when iter is not at end of XML file, but is at next ClientFile node
      if(std::strcmp((qtxml::_toString(elem->getTagName())).c_str(), "ClientFile")==0 && BEGINNING==false) {
	v1.push_back(options);
	options.reset();
	FIRSTHISTO=true;
	settings_map[fname]=v1; //assign vector of structs into settings map
	 v1.clear();
      }
      BEGINNING=false;
    } else if(elem==NULL) { //iter is at end of xml node
      v1.push_back(options);
      options.reset();
      FIRSTHISTO=true;
      settings_map[fname]=v1;
      v1.clear();
     }
  } //while statement
  
}


