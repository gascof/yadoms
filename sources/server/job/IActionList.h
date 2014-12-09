#pragma once

namespace job
{
   //-----------------------------------------------------
   ///\brief The list of actions interface
   //-----------------------------------------------------
   class IActionList
   {
   public:
      //-----------------------------------------------------
      ///\brief               Destructor
      //-----------------------------------------------------
      virtual ~IActionList() {}

      //-----------------------------------------------------
      ///\brief               Do all actions
      //-----------------------------------------------------
      virtual void doAll() = 0;
   };
	
} // namespace job	
	
	