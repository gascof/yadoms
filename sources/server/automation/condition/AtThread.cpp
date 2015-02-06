#include "stdafx.h"
#include "AtThread.h"
#include <shared/event/EventHandler.hpp>

namespace automation { namespace condition
{      

CAtThread::CAtThread()
   :CThreadBase("AtTimerThread")
{
}

CAtThread::~CAtThread()
{
}

void CAtThread::start(const boost::posix_time::ptime& timePoint, boost::function<void()> timeoutCallback)
{
   m_timePoint = timePoint;
   m_timeoutCallback = timeoutCallback;
   CThreadBase::start();
}

void CAtThread::doWork()
{
   enum { kTimerEventId = shared::event::kUserFirstId };
   shared::event::CEventHandler eventHandler;
   eventHandler.createTimePoint(kTimerEventId, m_timePoint);
   
   while (true)
   {
      // Wait for an event
      switch (eventHandler.waitForEvents())
      {
      case kTimerEventId:
         m_timeoutCallback();
         // Reset timer for next day
         m_timePoint += boost::posix_time::hours(24);
         break;
      default:
         BOOST_ASSERT_MSG(false, "Invalid event ID received");
         break;
      }
   }
}

} } // namespace automation::condition	
	
	