#pragma once

#include "IKeyword.h"
#include <shared/plugin/yPluginApi/IYPluginApi.h>
#include "../specificHistorizer/Color.h"

namespace yApi = shared::plugin::yPluginApi;

   //--------------------------------------------------------------
   /// \brief	The Load class
   //--------------------------------------------------------------
   class CForecastTomorrow : public IKeyword
   {
   public:
      //--------------------------------------------------------------
      /// \brief	                      Constructor
      /// \param[in] context              Yadoms APi context
	  /// \param[in] PluginName           The name of the plugin
	  /// \param[in] KeyWordName          The keyword name
      /// \note                           Use this constructor initialising the keyword for this plugin
      //--------------------------------------------------------------
      CForecastTomorrow( boost::shared_ptr<yApi::IYPluginApi> context, std::string PluginName,std::string KeyWordName, shared::CDataContainer details );

      //--------------------------------------------------------------
      /// \brief	                      Set the value of the keyword
      /// \param[in] Value                The new value
      //--------------------------------------------------------------
	  void SetValue ( std::string& Value );

      //--------------------------------------------------------------
      /// \brief	Destructor
      //--------------------------------------------------------------
      virtual ~CForecastTomorrow();

      // IKeyword implementation
	  virtual boost::shared_ptr<yApi::historization::IHistorizable> GetHistorizable() const;
      // [END] IKeyword implementation
      
   protected:

   private:
      //--------------------------------------------------------------
      /// \brief	The device name
      //--------------------------------------------------------------
      std::string m_PluginName;

      //--------------------------------------------------------------
      /// \brief	The period type
      //--------------------------------------------------------------
      boost::shared_ptr<teleInfoUSB::specificHistorizers::CColor> m_forecastPeriod;

      //--------------------------------------------------------------
      /// \brief	The map of returned value to the IHM
      //--------------------------------------------------------------
	  typedef std::map<std::string, unsigned int> EnumColorMap;
   };