#include "FWCore/MessageLogger/interface/MessageLogger.h"

/** 
    @author R.Bainbridge
    @brief Creates MessageLoggerPresence for use with stand-alone applications. 
*/
class MessageLoggerInstance {
  
 public:
  
  static MessageLoggerInstance* instance();
  static void deleteInstance();
  ~MessageLoggerInstance();
  
 private:
  
  static MessageLoggerInstance* instance_;
  MessageLoggerInstance();
  void create();
  
};

