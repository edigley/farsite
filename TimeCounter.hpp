#include <string>
#include <time.h>


class TimeCounter{

public:

   static TimeCounter* Instance();

//    bool openLogFile(std::string logFile);
// 
//    void writeToLogFile();
// 
//    bool closeLogFile();

   float getTime();
   float getTimeS();
   
   void startT();
   void startTS();
   void stopT();
   void stopTS();
   

private:

   TimeCounter(){};  // Private so that it can  not be called

   TimeCounter(TimeCounter const&){};             // copy constructor is private

   TimeCounter& operator=(TimeCounter const&){};  // assignment operator is private

   static TimeCounter* m_pInstance;

protected:  
   struct timespec t0,t1,st0,st1;
   long AccTime_sec,AccTimeS_sec,AccTime_nsec,AccTimeS_nsec;
//   time_t t0,t1,st0,st1;

};
 
