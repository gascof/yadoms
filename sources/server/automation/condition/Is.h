#pragma once
#include "ICondition.h"
#include <shared/DataContainer.h>
#include "../INotificationObserverForRulesManager.h"
#include "IConditionRoot.h"
#include "IKeywordUpdater.h"
#include "../../database/IAcquisitionRequester.h"

namespace automation { namespace condition
{
   //-----------------------------------------------------
   ///\brief               Available operators
   //-----------------------------------------------------
   DECLARE_ENUM_HEADER(EOperators,
      ((Equal))
      ((Different))
      ((Lower))
      ((LowerOrEqual))
      ((Greater))
      ((GreaterOrEqual))
   )
      
   //-----------------------------------------------------
   ///\brief The IS operator
   //-----------------------------------------------------
   class CIs : public ICondition, public IKeywordUpdater, public boost::enable_shared_from_this<CIs>
   {
   public:
      //-----------------------------------------------------
      ///\brief               Constructor
      ///\param[in] configuration Condition configuration node
      ///\param[in] dbAcquisitionRequester  Database acquisition requester
      ///\throw shared::exception::CEmptyResult if associated keyword is not found
      //-----------------------------------------------------
      CIs(const shared::CDataContainer& configuration, boost::shared_ptr<database::IAcquisitionRequester> dbAcquisitionRequester);

      //-----------------------------------------------------
      ///\brief               Destructor
      //-----------------------------------------------------
      virtual ~CIs();

   protected:
      // ICondition Implementation
      virtual bool eval() const;
      virtual void registerToNotificationCenter(boost::shared_ptr<INotificationObserverForRulesManager> notificationObserver, boost::shared_ptr<IConditionRootUpdater> conditionRootNotifier);
      virtual void unregisterFromNotificationCenter(boost::shared_ptr<INotificationObserverForRulesManager> notificationObserver);
      // [END] ICondition Implementation

      // IKeywordUpdater Implementation
      virtual int getKeywordId() const;
      virtual void onKeywordStateChange(const std::string& state);
      // [END] IKeywordUpdater Implementation

      //-----------------------------------------------------
      ///\brief               Convert value to double to perform '<' or '>' comparaisons
      //-----------------------------------------------------
      static double toDouble(const std::string& value);

   private:
      //-----------------------------------------------------
      ///\brief               Keyword ID
      //-----------------------------------------------------
      const int m_keywordId;

      //-----------------------------------------------------
      ///\brief               Operator
      //-----------------------------------------------------
      const EOperators m_operator;

      //-----------------------------------------------------
      ///\brief               Current value
      //-----------------------------------------------------
      std::string m_value;

      //-----------------------------------------------------
      ///\brief               Expected value
      //-----------------------------------------------------
      const std::string m_expectedValue;
   };
	
} } // namespace automation::condition	
	
	