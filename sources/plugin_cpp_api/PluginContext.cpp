#include "stdafx.h"
#include "PluginContext.h"
#include "ApiImplementation.h"
#include "CommandLine.h"
#include <shared/currentTime/Local.h>

//TODO      Ajouter ce lien dans le wiki plugin/how to debug my plugin : https://msdn.microsoft.com/en-us/library/a329t4ed(v=vs.90).aspx
shared::currentTime::Provider timeProvider(boost::make_shared<shared::currentTime::Local>());

namespace plugin_cpp_api
{
   int doMain(int argc, char **argv, boost::shared_ptr<IPlugin> plugin)
   {
      try
      {
         auto pluginContext = boost::make_shared<CPluginContext>(argc, argv, plugin);
         pluginContext->run();
         return static_cast<int>(pluginContext->getReturnCode());
      }
      catch (std::invalid_argument& e)
      {
         std::cerr << "Unable to start plugin : " << e.what() << std::endl;
         return kStartError;
      }
      catch (...)
      {
         std::cerr << "Plugin crashed" << std::endl;
         return kRuntimeError;
      }
   }


   CPluginContext::CPluginContext(int argc, char** argv, boost::shared_ptr<IPlugin> plugin)
      : m_commandLine(boost::make_shared<CCommandLine>(argc, argv)),
        m_plugin(plugin),
        m_returnCode(kOk)
   {
   }

   CPluginContext::~CPluginContext()
   {
   }

   void CPluginContext::run()
   {
      auto api = boost::make_shared<CApiImplementation>();

      try
      {
         openMessageQueues();

         api->setSendingMessageQueue(m_sendMessageQueue);

         m_msgReceiverThread = boost::thread(&CPluginContext::msgReceiverThreaded, this, api);

         api->waitInitialized();

         std::cout << api->getInformation()->getType() << " started" << std::endl;

         // Execute plugin code
         m_plugin->doWork(api);

         if (!api->stopRequested())
         {
            m_returnCode = kUnexpectedStop;
            std::cerr << api->getInformation()->getType() << " has stopped itself." << std::endl;
         }

         // Normal stop
         std::cout << api->getInformation()->getType() << " is stopped" << std::endl;
         m_returnCode = kOk;
      }
      catch (std::exception& e)
      {
         m_returnCode = kRuntimeError;
         std::cerr << api->getInformation()->getType() << " crashed with exception : " << e.what();
      }
      catch (...)
      {
         m_returnCode = kRuntimeError;
         std::cerr << api->getInformation()->getType() << " crashed with unknown exception" << std::endl;
      }

      m_msgReceiverThread.interrupt();
      m_msgReceiverThread.timed_join(boost::posix_time::seconds(20));

      closeMessageQueues();
   }

   EReturnCode CPluginContext::getReturnCode() const
   {
      return m_returnCode;
   }

   void CPluginContext::openMessageQueues() const
   {
      // Verify that the version of the library that we linked against is
      // compatible with the version of the headers we compiled against.
      GOOGLE_PROTOBUF_VERIFY_VERSION;

      try
      {
         const auto sendMessageQueueId(m_commandLine->yPluginApiAccessorId() + ".toYadoms");
         const auto receiveMessageQueueId(m_commandLine->yPluginApiAccessorId() + ".toPlugin");

         std::cout << "Opening message queues id " << m_commandLine->yPluginApiAccessorId() << std::endl;

         m_sendMessageQueue = boost::make_shared<boost::interprocess::message_queue>(boost::interprocess::open_only, sendMessageQueueId.c_str());
         m_receiveMessageQueue = boost::make_shared<boost::interprocess::message_queue>(boost::interprocess::open_only, receiveMessageQueueId.c_str());
      }
      catch (boost::interprocess::interprocess_exception& ex)
      {
         throw std::runtime_error(std::string("CPluginContext::msgReceiverThreaded : Error opening message queue, ") + ex.what());
      }
      catch (...)
      {
         throw std::runtime_error("CPluginContext::msgReceiverThreaded : Unknown error");
      }
   }

   void CPluginContext::closeMessageQueues()
   {
      std::cout << "Close message queues" << std::endl;

      // Delete all global objects allocated by libprotobuf.
      google::protobuf::ShutdownProtobufLibrary();
   }

   void CPluginContext::msgReceiverThreaded(boost::shared_ptr<CApiImplementation> api) const
   {
      auto message(boost::make_shared<unsigned char[]>(m_receiveMessageQueue->get_max_msg_size()));
      size_t messageSize;
      unsigned int messagePriority;

      try
      {
         while (true)
         {
            try
            {
               // boost::interprocess::message_queue::receive is not responding to boost thread interruption, so we need to do some
               // polling and call boost::this_thread::interruption_point to exit properly
               // Note that boost::interprocess::message_queue::timed_receive requires universal time to work (can not use shared::currentTime::Provider)
               auto messageWasReceived = m_receiveMessageQueue->timed_receive(message.get(), m_receiveMessageQueue->get_max_msg_size(), messageSize, messagePriority,
                  boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(1)));
               boost::this_thread::interruption_point();

               if (messageWasReceived)
                  api->onReceive(message, messageSize);
            }
            catch (shared::exception::CInvalidParameter& ex)
            {
               std::cerr << "Error receiving/processing queue message : " << ex.what() << std::endl;
            }
         }
      }
      catch (boost::thread_interrupted&)
      {
      }
   }

} // namespace plugin_cpp_api