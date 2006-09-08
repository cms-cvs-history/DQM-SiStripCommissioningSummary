#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <string>
/** 
    @author R.Bainbridge
    @brief Creates MessageLoggerPresence for use with stand-alone applications. 
*/
class MessageLoggerInstance {
  
 public:
  
  static MessageLoggerInstance* instance();
  static void deleteInstance();
  ~MessageLoggerInstance();

  inline void configurationFile( const std::string& filename );
  
  void doRealWork();

 private:
  
  void useMessageLogger();
  void putCodeHere();
  
  static MessageLoggerInstance* instance_;
  MessageLoggerInstance();

  std::string cfgFile_;
  
};

void MessageLoggerInstance::configurationFile( const std::string& filename ) { cfgFile_ = filename; }

