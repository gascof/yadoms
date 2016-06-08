#include "stdafx.h"
#include "AsyncTcpClientPimpl.h"
#include <shared/Log.h>
#include <shared/exception/InvalidParameter.hpp>
#include "PortException.hpp"
#include "Buffer.hpp"

namespace shared { namespace communication {

CAsyncTcpClientPimpl::CAsyncTcpClientPimpl(
   const std::string& serverAddress,
   const std::string& serverPort,
   boost::posix_time::time_duration connectRetryDelay)
   :m_boostSocket(m_ioService),
   m_serverAdressResolver(m_ioService),
   m_serverAdressResolverQuery(serverAddress, serverPort),
   m_asyncReadBuffer(512),
   m_connectStateEventHandler(NULL),
   m_connectStateEventId(event::kNoEvent),
   m_connectRetryDelay(connectRetryDelay),
   m_connectRetryTimer(m_ioService)
{
}

CAsyncTcpClientPimpl::~CAsyncTcpClientPimpl()
{
   stop();
}

void CAsyncTcpClientPimpl::setReceiveBufferMaxSize(std::size_t size)
{
   m_asyncReadBuffer.resize(size);
}

void CAsyncTcpClientPimpl::start()
{
   if (!!m_asyncThread)
      return;  // Already started

   // Try to connect
   tryConnect();
   m_asyncThread.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_ioService)));
}

void CAsyncTcpClientPimpl::stop()
{
   if (!m_asyncThread)
      return;  // Already stopped

   disconnect();

   m_ioService.stop();
   m_asyncThread->join();
   m_asyncThread.reset();
}

void CAsyncTcpClientPimpl::disconnect()
{
   if (!m_boostSocket.is_open())
      return;

   // Close the port
   try
   {
      m_boostSocket.close();
   }
   catch (boost::system::system_error& e)
   {
      YADOMS_LOG(error) << "Failed to close socket " << e.what();
      return;
   }
}

bool CAsyncTcpClientPimpl::isConnected() const
{
   return m_boostSocket.is_open();
}

void CAsyncTcpClientPimpl::subscribeForConnectionEvents(event::CEventHandler& forEventHandler, int forId)
{
   m_connectStateEventHandler = &forEventHandler;
   m_connectStateEventId = forId;
}

void CAsyncTcpClientPimpl::setReceiveBufferHandler(boost::shared_ptr<IReceiveBufferHandler> handler)
{
   m_receiveBufferHandler = handler;
}

void CAsyncTcpClientPimpl::flush()
{
   if (!!m_receiveBufferHandler)
      m_receiveBufferHandler->flush();
}

void CAsyncTcpClientPimpl::reconnectTimerHandler(const boost::system::error_code& error)
{
   if (error != 0)
      throw exception::CException("Error code should be 0 here");

   tryConnect();
}

void CAsyncTcpClientPimpl::tryConnect()
{
   if (!isConnected())
      YADOMS_LOG(warning) << "Already connected";

   m_serverAdressResolver.async_resolve(m_serverAdressResolverQuery,
      boost::bind(&CAsyncTcpClientPimpl::handleEndPointResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void CAsyncTcpClientPimpl::handleEndPointResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator iterator)
{
   if (error)
   {
      // Fail to reconnect, retry after a certain delay
      m_connectRetryTimer.expires_from_now(m_connectRetryDelay);
      m_connectRetryTimer.async_wait(boost::bind(&CAsyncTcpClientPimpl::reconnectTimerHandler, this, boost::asio::placeholders::error));
      return;
   }

   // End point is solved, try to connect now
   boost::asio::async_connect(m_boostSocket, iterator,
      boost::bind(&CAsyncTcpClientPimpl::handleTryConnect, this, boost::asio::placeholders::error));
}

void CAsyncTcpClientPimpl::handleTryConnect(const boost::system::error_code& error)
{
   if (error)
   {
      // Fail to reconnect, retry after a certain delay
      m_connectRetryTimer.expires_from_now(m_connectRetryDelay);
      m_connectRetryTimer.async_wait(boost::bind(&CAsyncTcpClientPimpl::reconnectTimerHandler, this, boost::asio::placeholders::error));
      return;
   }

   // Connected
   notifyEventHandler(true);

   // Start listening on the port
   startRead();
}

void CAsyncTcpClientPimpl::startRead()
{
   // Start an asynchronous read and call readCompleted when it completes or fails 
   m_boostSocket.async_read_some(boost::asio::buffer(m_asyncReadBuffer.begin(), m_asyncReadBuffer.size()),
      boost::bind(&CAsyncTcpClientPimpl::readCompleted, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void CAsyncTcpClientPimpl::readCompleted(const boost::system::error_code& error, std::size_t bytesTransferred)
{
   if (error)
   {
      // boost::asio::error::operation_aborted is fired when stop is required
      if (error == boost::asio::error::operation_aborted)
         return;     // Normal stop

      // Error ==> disconnecting
      YADOMS_LOG(error) << "Socket read error : " << error.message();
      disconnect();
      notifyEventHandler(false);
      return;
   }

   // Read OK
   CByteBuffer buffer(bytesTransferred);
   memcpy(buffer.begin(), m_asyncReadBuffer.begin(), bytesTransferred);

   if (!!m_receiveBufferHandler)
      m_receiveBufferHandler->push(buffer);

   // Restart read
   startRead();
}

void CAsyncTcpClientPimpl::send(const CByteBuffer& buffer)
{
   try
   {
      m_boostSocket.write_some(boost::asio::const_buffers_1(buffer.begin(), buffer.size()));
   }
   catch (boost::system::system_error& e)
   {
      // boost::asio::error::eof is the normal stop
      if (e.code() != boost::asio::error::eof)
      {
         YADOMS_LOG(error) << "Socket write error : " << e.what();
         disconnect();
      }

      notifyEventHandler(false);

      throw CPortException(
         (e.code() == boost::asio::error::eof) ? CPortException::kConnectionClosed : CPortException::kConnectionError,
         e.what());
   }
}

void CAsyncTcpClientPimpl::notifyEventHandler(bool isConnected)
{
   if (m_connectStateEventHandler)
      m_connectStateEventHandler->postEvent<bool>(m_connectStateEventId, isConnected);
}

} } // namespace shared::communication
