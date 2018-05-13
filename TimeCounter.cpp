#include "TimeCounter.hpp"

 

// Global static pointer used to ensure a single instance of the class.

TimeCounter* TimeCounter::m_pInstance = NULL; 

 

/** This function is called to create an instance of the class.

    Calling the constructor publicly is not allowed. The constructor

    is private and is only called by this Instance function.

*/

   

TimeCounter* TimeCounter::Instance()
{

   if (!m_pInstance){   // Only allow one instance of class to be generated.
      m_pInstance = new TimeCounter;
      m_pInstance->AccTime_sec=0;
      m_pInstance->AccTime_nsec=0;
//      m_pInstance->t0=0.0f;
//      m_pInstance->t1=0.0f;
      m_pInstance->AccTimeS_sec=0;
      m_pInstance->AccTimeS_nsec=0;
//      m_pInstance->st0=0.0f;
//      m_pInstance->st1=0.0f;
   }

   return m_pInstance;

}


float TimeCounter::getTime(){
	return ((float)TimeCounter::Instance()->AccTime_sec+ (float)TimeCounter::Instance()->AccTime_nsec/1000000000);
}
float TimeCounter::getTimeS(){
	return ((float)TimeCounter::Instance()->AccTimeS_sec+ (float)TimeCounter::Instance()->AccTimeS_nsec/1000000000);
}
void TimeCounter::startTS()
{
	struct timespec temp;
	clock_gettime(CLOCK_REALTIME, &temp);
	TimeCounter::Instance()->st0.tv_sec=temp.tv_sec;
	TimeCounter::Instance()->st0.tv_nsec=temp.tv_nsec;
}
void TimeCounter::stopTS()
{
	struct timespec temp;
	clock_gettime(CLOCK_REALTIME, &temp);
	TimeCounter::Instance()->AccTimeS_sec += (temp.tv_sec-TimeCounter::Instance()->st0.tv_sec);
        TimeCounter::Instance()->AccTimeS_nsec += (temp.tv_nsec-TimeCounter::Instance()->st0.tv_nsec);
//	TimeCounter::Instance()->AccTimeS += ( ((float)temp.tv_sec + (float)temp.tv_nsec/1000000000) - ((float)TimeCounter::Instance()->st0.tv_sec + (float)TimeCounter::Instance()->st0.tv_nsec/1000000000));
}

void TimeCounter::startT()
{
	struct timespec temp;
	clock_gettime(CLOCK_REALTIME, &temp);
	TimeCounter::Instance()->t0.tv_sec=temp.tv_sec;
	TimeCounter::Instance()->t0.tv_nsec=temp.tv_nsec;
}
  
void TimeCounter::stopT()
{
	struct timespec temp;
	clock_gettime(CLOCK_REALTIME, &temp);
	TimeCounter::Instance()->AccTime_sec += (temp.tv_sec-TimeCounter::Instance()->t0.tv_sec);
	TimeCounter::Instance()->AccTime_nsec += (temp.tv_nsec-TimeCounter::Instance()->t0.tv_nsec);
//	TimeCounter::Instance()->AccTime += ( ((float)temp.tv_sec + (float)temp.tv_nsec/1000000000) - ((float)TimeCounter::Instance()->t0.tv_sec + (float)TimeCounter::Instance()->t0.tv_nsec/1000000000));
}


 
