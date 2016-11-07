#include "stdafx.h"
#include "HttpMethods.h"
#include <Poco/Net/HTTPSession.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include "Poco/StreamCopier.h"
#include <Poco/URI.h>
#include <shared/exception/Exception.hpp>
#include <shared/Log.h>


namespace http
{
   shared::CDataContainer CHttpMethods::SendGetRequest(const std::string & url, shared::CDataContainer & parameters)
   {
      try
      {
         std::map<std::string, std::string> mapParameters = parameters.getAsMap();
         Poco::URI uri(url);

         if (!parameters.empty())
         {
            for (std::map<std::string, std::string>::iterator parametersIterator = mapParameters.begin(); parametersIterator != mapParameters.end(); ++parametersIterator)
               uri.addQueryParameter(parametersIterator->first, parametersIterator->second);
         }

         Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
         Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);

         session.sendRequest(request);

         Poco::Net::HTTPResponse response;
         auto& rs = session.receiveResponse(response);

         if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
         {
            std::ostringstream oss;
            Poco::StreamCopier::copyStream(rs, oss);
            return shared::CDataContainer(oss.str());
         }
         else
         {
            auto message = (boost::format("Invalid HTTP result : %1%") % response.getReason()).str();
            std::cerr << message << std::endl;
            throw shared::exception::CException(message);
         }
      }
      catch (Poco::Exception& e) 
      {
         auto message = (boost::format("Fail to send get http request \"%1%\" : %2%") % url % e.message()).str();
         std::cerr << message << std::endl;
         throw shared::exception::CException(message);
      }
      return shared::CDataContainer();
   }
} // namespace http
