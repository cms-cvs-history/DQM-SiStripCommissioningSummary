/** \file
 *
 *  Implementation of ConfigParser
 *
 *  $Date: 2006/12/04 16:16:51 $
 *  Revision: 1.0
 *  \author Puneeth Kalavase
 */

#include "DQM/SiStripCommissioningSummary/bin/stubs/ConfigParser.h"
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
#include <stdexcept>
#include <map>
#include <vector>

using namespace std;
using namespace xercesc;

// -----------------------------------------------------------------------------
//
const string ConfigParser::rootTag_ = "root";
const string ConfigParser::runTypeTag_ = "RunType";
const string ConfigParser::runTypeAttr_ = "name";
const string ConfigParser::summaryPlotTag_ = "SummaryPlot";
const string ConfigParser::monitorableAttr_ = "monitorable";
const string ConfigParser::presentationAttr_ = "presentation";
const string ConfigParser::viewAttr_ = "view";
const string ConfigParser::levelAttr_ = "level";
const string ConfigParser::granularityAttr_ = "granularity";

// -----------------------------------------------------------------------------
//
ConfigParser::ConfigParser() {
  summaryPlotMap_.clear();
  try { XMLPlatformUtils::Initialize(); }
  catch ( const XMLException &f ) {
    throw( runtime_error("Standard pool exception : Fatal Error on pool::TrivialFileCatalog") );
  }
}

// -----------------------------------------------------------------------------
// 
ConfigParser::SummaryPlot::SummaryPlot() :
  mon_( sistrip::UNKNOWN_MONITORABLE ),
  pres_( sistrip::UNKNOWN_PRESENTATION ),
  view_( sistrip::UNKNOWN_VIEW ),
  gran_( sistrip::UNKNOWN_GRAN ),
  level_("")
{;}

// -----------------------------------------------------------------------------
// 
void ConfigParser::SummaryPlot::reset() {
  mon_ =  sistrip::UNKNOWN_MONITORABLE;
  pres_ =  sistrip::UNKNOWN_PRESENTATION;
  view_ = sistrip::UNKNOWN_VIEW;
  gran_ =  sistrip::UNKNOWN_GRAN;
  level_ = "";
}

// -----------------------------------------------------------------------------
// 
void ConfigParser::SummaryPlot::print( stringstream& ss ) const {
  ss << "[ConfigParser::SummaryPlot::" << __func__ << "]" << endl
     << " Monitorable:  " <<  SiStripEnumsAndStrings::monitorable(mon_) << endl
     << " Presentation: " << SiStripEnumsAndStrings::presentation(pres_) << endl
     << " View:         " << SiStripEnumsAndStrings::view(view_) << endl
     << " TopLevelDir:  " << level_ << endl
     << " Granularity:  " << SiStripEnumsAndStrings::granularity(gran_) << endl;
}

// -----------------------------------------------------------------------------
// 
void ConfigParser::SummaryPlot::checkView() {

  sistrip::View check = SiStripEnumsAndStrings::view( level_ );
  
  if ( check != view_ ) {
    stringstream ss;
    ss << "[ConfigParser::SummaryPlot::" << __func__ << "]"
       << " Mismatch between level_ and view_ member data!";
    if ( check != sistrip::UNKNOWN_VIEW ) {
      ss << " Changing view_ from "
	 << SiStripEnumsAndStrings::view( view_ )
	 << " to " 
	 << SiStripEnumsAndStrings::view( check ); 
      view_ = check;
    } else {
      string temp = SiStripEnumsAndStrings::view( view_ ) + "/" + level_;
      ss << " Changing level_ from "
	 << level_ 
	 << " to " 
	 << temp;
      level_ = temp;
    }
    cerr << ss.str() << endl;
  }
  
}

// -----------------------------------------------------------------------------
//
ostream& operator<< ( std::ostream& os, const ConfigParser::SummaryPlot& summary ) {
  stringstream ss;
  summary.print(ss);
  os << ss.str();
  return os;
}

// -----------------------------------------------------------------------------
//
const vector<ConfigParser::SummaryPlot>& ConfigParser::summaryPlots( const sistrip::RunType& run_type ) {
  
  if( summaryPlotMap_.empty() ) {
    cout << "You have not called the parseXML function,"
	 << " or your XML file is erronious" << endl;
  }
  if( summaryPlotMap_.find( run_type ) != summaryPlotMap_.end() ) {
    return summaryPlotMap_[run_type];
  } else {
    static vector<SummaryPlot> blank;
    return blank;
  }

}

// -----------------------------------------------------------------------------
//
void ConfigParser::parseXML( const string& f ) {
  
  summaryPlotMap_.clear();
  ConfigParser::SummaryPlot summary;
  
  try {

    // Create parser and open XML document
    getDocument(f);
    
    // Retrieve root element
    DOMElement* root = this->doc->getDocumentElement();
    if( !root ) { 
      stringstream ss;
      ss << "[ConfigParser::" << __func__ << "]"
	 << " Unable to find any elements!"
	 << " Empty xml document?...";
      throw( runtime_error( ss.str() ) ); 
    }

    // Check on "root" tag
    if( !XMLString::equals( root->getTagName(), XMLString::transcode(rootTag_.c_str()) ) ) {
      cout << "[ConfigParser::" << __func__ << "]"
	   << " Did not find \"" << rootTag_ << "\" tag! " 
	   << " Tag name is "
	   << XMLString::transcode(root->getNodeName()) 
	   << endl;
      return;
    }
        
    // Retrieve nodes in xml document
    DOMNodeList* nodes = root->getChildNodes();
    if ( nodes->getLength() == 0 ) { 
      stringstream ss;
      ss << "[ConfigParser::" << __func__ << "]"
	 << " Unable to find any children nodes!"
	 << " Empty xml document?...";
      throw( runtime_error( ss.str() ) ); 
      return;
    }


    cout << "[ConfigParser::" << __func__ << "]"
	 << " Found \"" << rootTag_ << "\" tag!" 
	 << endl;
    
    cout << "[ConfigParser::" << __func__ << "]"
	 << " Found " << nodes->getLength()
	 << " children nodes!" 
	 << endl;
    
    // Iterate through nodes
    for( XMLSize_t inode = 0; inode < nodes->getLength(); ++inode ) {

      // Check on whether node is element
      DOMNode* node = nodes->item(inode);
      if( node->getNodeType() &&
	  node->getNodeType() == DOMNode::ELEMENT_NODE ) {
	
	DOMElement* element = dynamic_cast<DOMElement*>( node );
	if ( !element ) { continue; }

	if( XMLString::equals( element->getTagName(), 
			       XMLString::transcode(runTypeTag_.c_str()) ) ) {
	  
	  const XMLCh* attr = element->getAttribute( XMLString::transcode(runTypeAttr_.c_str()) );
	  sistrip::RunType run_type = SiStripEnumsAndStrings::runType( XMLString::transcode(attr) );

	  cout << "[ConfigParser::" << __func__ << "]"
	       << " Found \"" << runTypeTag_ << "\" tag!" << endl
	       << "  with tag name \"" << XMLString::transcode(element->getNodeName()) << "\"" << endl
	       << "  and attr \"" << runTypeAttr_ << "\" with value \"" << XMLString::transcode(attr) << "\"" << endl;

	  // Retrieve nodes in xml document
	  DOMNodeList* children = node->getChildNodes();
	  if ( nodes->getLength() == 0 ) { 
	    stringstream ss;
	    ss << "[ConfigParser::" << __func__ << "]"
	       << " Unable to find any children nodes!"
	       << " Empty xml document?...";
	    throw( runtime_error( ss.str() ) ); 
	    return;
	  }

	  // Iterate through nodes
	  for( XMLSize_t jnode = 0; jnode < children->getLength(); ++jnode ) {

	    // Check on whether node is element
	    DOMNode* child = children->item(jnode);
	    if( child->getNodeType() &&
		child->getNodeType() == DOMNode::ELEMENT_NODE ) {
	
	      DOMElement* elem = dynamic_cast<DOMElement*>( child );
	      if ( !elem ) { continue; }

	      if( XMLString::equals( elem->getTagName(), 
				     XMLString::transcode(summaryPlotTag_.c_str()) ) ) {
	  	
		const XMLCh* mon = elem->getAttribute( XMLString::transcode(monitorableAttr_.c_str()) );
		const XMLCh* pres = elem->getAttribute( XMLString::transcode(presentationAttr_.c_str()) );
		const XMLCh* view = elem->getAttribute( XMLString::transcode(viewAttr_.c_str()) );
		const XMLCh* level = elem->getAttribute( XMLString::transcode(levelAttr_.c_str()) );
		const XMLCh* gran = elem->getAttribute( XMLString::transcode(granularityAttr_.c_str()) );
  
		cout << "[ConfigParser::" << __func__ << "]"
		     << " Found \"" << summaryPlotTag_ << "\" tag!" << endl
		     << "  with tag name \"" << XMLString::transcode(elem->getNodeName()) << "\"" << endl
		     << "  and attr \"" << monitorableAttr_ << "\" with value \"" << XMLString::transcode(mon) << "\"" << endl
		     << "  and attr \"" << presentationAttr_ << "\" with value \"" << XMLString::transcode(pres) << "\"" << endl
		     << "  and attr \"" << viewAttr_ << "\" with value \"" << XMLString::transcode(view) << "\"" << endl
		     << "  and attr \"" << levelAttr_ << "\" with value \"" << XMLString::transcode(level) << "\"" << endl
		     << "  and attr \"" << granularityAttr_ << "\" with value \"" << XMLString::transcode(gran) << "\"" << endl;

		// Update SummaryPlot object and push back into map
		summary.reset();
		summary.mon_ = SiStripEnumsAndStrings::monitorable( XMLString::transcode(mon) );
		summary.pres_ = SiStripEnumsAndStrings::presentation( XMLString::transcode(pres) );
		summary.view_ = SiStripEnumsAndStrings::view( XMLString::transcode(view) );
		summary.gran_ = SiStripEnumsAndStrings::granularity( XMLString::transcode(gran) );
		summary.level_ = XMLString::transcode(level);
		summary.checkView();
		summaryPlotMap_[run_type].push_back(summary);
		
	      }
	    }
	  }
	  
	}
      }
    }

  }
  catch( XMLException& e ) {
    char* message = XMLString::transcode(e.getMessage());
    ostringstream ss;
    ss << "[ConfigParser::" << __func__ << "]"
       << " Error parsing file: " << message << flush;
    XMLString::release( &message );
  }
  
}




