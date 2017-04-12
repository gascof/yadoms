#include "stdafx.h"
#include "Transceiver.h"
#include <shared/DataContainer.h>
#include <shared/exception/InvalidParameter.hpp>
#include "rfxcomMessages/Ack.h"
#include "rfxcomMessages/Barometric.h"
#include "rfxcomMessages/Blinds1.h"
#include "rfxcomMessages/Bbq.h"
#include "rfxcomMessages/Camera1.h"
#include "rfxcomMessages/Cartelectronic.h"
#include "rfxcomMessages/Chime.h"
#include "rfxcomMessages/Current.h"
#include "rfxcomMessages/CurrentEnergy.h"
#include "rfxcomMessages/Curtain1.h"
#include "rfxcomMessages/DateTime.h"
#include "rfxcomMessages/Fan.h"
#include "rfxcomMessages/Energy.h"
#include "rfxcomMessages/FS20.h"
#include "rfxcomMessages/HomeConfort.h"
#include "rfxcomMessages/Humidity.h"
#include "rfxcomMessages/Lighting1.h"
#include "rfxcomMessages/Lighting2.h"
#include "rfxcomMessages/Lighting3.h"
#include "rfxcomMessages/Lighting4.h"
#include "rfxcomMessages/Lighting5.h"
#include "rfxcomMessages/Lighting6.h"
#include "rfxcomMessages/Power.h"
#include "rfxcomMessages/Radiator1.h"
#include "rfxcomMessages/Rain.h"
#include "rfxcomMessages/Remote.h"
#include "rfxcomMessages/RFXMeter.h"
#include "rfxcomMessages/RFXSensor.h"
#include "rfxcomMessages/Rfy.h"
#include "rfxcomMessages/Security1.h"
#include "rfxcomMessages/Security2.h"
#include "rfxcomMessages/Temp.h"
#include "rfxcomMessages/TempRain.h"
#include "rfxcomMessages/TempHumidity.h"
#include "rfxcomMessages/TempHumidityBarometric.h"
#include "rfxcomMessages/Thermostat1.h"
#include "rfxcomMessages/Thermostat2.h"
#include "rfxcomMessages/Thermostat3.h"
#include "rfxcomMessages/Thermostat4.h"
#include "rfxcomMessages/TransceiverStatus.h"
#include "rfxcomMessages/UV.h"
#include "rfxcomMessages/Weight.h"
#include "rfxcomMessages/Wind.h"
#include "IncrementSequenceNumber.h"
#include "ManuallyDeviceCreationException.hpp"
#include <shared/Log.h>

//
// =======================================================================
// RFXCOM implementation
// =======================================================================
// This RFXCom support was developped for :
// - Sepcifications "RFXtrx SDK.pdf" : Version 9.14 Jan 12, 2017
// - RFXtrx.h : version 9.13
// =======================================================================
//

CTransceiver::CTransceiver()
   : m_seqNumberProvider(boost::make_shared<CIncrementSequenceNumber>()),
     m_unsecuredProtocolFilters(createUnsecuredProtocolFilters())
{
}

CTransceiver::~CTransceiver()
{
}

std::map<int, boost::shared_ptr<IUnsecuredProtocolFilter>> CTransceiver::createUnsecuredProtocolFilters()
{
   std::map<int, boost::shared_ptr<IUnsecuredProtocolFilter>> filters;
   filters[pTypeCURRENTENERGY] = rfxcomMessages::CCurrentEnergy::createFilter();
   return filters;
}


shared::communication::CByteBuffer CTransceiver::buildResetCmd() const
{
   // Raz sequence number
   m_seqNumberProvider->reset();

   RBUF request;
   MEMCLEAR(request.ICMND); // For better performance, just clear the needed sub-structure of RBUF

   request.ICMND.packetlength = ENCODE_PACKET_LENGTH(ICMND);
   request.ICMND.packettype = pTypeInterfaceControl;
   request.ICMND.subtype = sTypeInterfaceCommand;
   request.ICMND.seqnbr = m_seqNumberProvider->last();
   request.ICMND.cmnd = cmdRESET;

   return toBuffer(request, GET_RBUF_STRUCT_SIZE(ICMND));
}

shared::communication::CByteBuffer CTransceiver::buildGetStatusCmd() const
{
   RBUF request;
   MEMCLEAR(request.ICMND); // For better performance, just clear the needed sub-structure of RBUF

   request.ICMND.packetlength = ENCODE_PACKET_LENGTH(ICMND);
   request.ICMND.packettype = pTypeInterfaceControl;
   request.ICMND.subtype = sTypeInterfaceCommand;
   request.ICMND.seqnbr = m_seqNumberProvider->next();
   request.ICMND.cmnd = cmdSTATUS;

   return toBuffer(request, GET_RBUF_STRUCT_SIZE(ICMND));
}

shared::communication::CByteBuffer CTransceiver::buildSetModeCmd(unsigned char frequency,
                                                                 const IRfxcomConfiguration& configuration) const
{
   RBUF request;
   MEMCLEAR(request.ICMND); // For better performance, just clear the needed sub-structure of RBUF

   request.ICMND.packetlength = ENCODE_PACKET_LENGTH(ICMND);
   request.ICMND.packettype = pTypeInterfaceControl;
   request.ICMND.subtype = sTypeInterfaceCommand;
   request.ICMND.seqnbr = m_seqNumberProvider->next();
   request.ICMND.cmnd = cmdSETMODE;

   // Frequency
   request.ICMND.freqsel = frequency;

   // Frequency
   request.ICMND.xmitpwr = 0; // Not yet supported by RFXCom

   // Add protocols activation
   request.ICMND.msg3 = 0;
   if (configuration.isAEenabled()) request.ICMND.msg3 |= msg3_AE;
   if (configuration.isRUBICSONenabled()) request.ICMND.msg3 |= msg3_RUBICSON;
   if (configuration.isFINEOFFSETenabled()) request.ICMND.msg3 |= msg3_FINEOFFSET;
   if (configuration.isLIGHTING4enabled()) request.ICMND.msg3 |= msg3_LIGHTING4;
   if (configuration.isRSLenabled()) request.ICMND.msg3 |= msg3_RSL;
   if (configuration.isSXenabled()) request.ICMND.msg3 |= msg3_SX;
   if (configuration.isIMAGINTRONIXenabled()) request.ICMND.msg3 |= msg3_IMAGINTRONIX;
   if (configuration.isUNDECODEDenabled()) request.ICMND.msg3 |= msg3_undec;
   request.ICMND.msg4 = 0;
   if (configuration.isMERTIKenabled()) request.ICMND.msg4 |= msg4_MERTIK;
   if (configuration.isLWRFenabled()) request.ICMND.msg4 |= msg4_AD;
   if (configuration.isHIDEKIenabled()) request.ICMND.msg4 |= msg4_HID;
   if (configuration.isLACROSSEenabled()) request.ICMND.msg4 |= msg4_LCROS;
   if (configuration.isFS20enabled()) request.ICMND.msg4 |= msg4_FS20;
   if (configuration.isPROGUARDenabled()) request.ICMND.msg4 |= msg4_PROGUARD;
   if (configuration.isBLINDST0enabled()) request.ICMND.msg4 |= msg4_BLINDST0;
   if (configuration.isBLINDST1enabled()) request.ICMND.msg4 |= msg4_BLINDST1;
   request.ICMND.msg5 = 0;
   if (configuration.isX10enabled()) request.ICMND.msg5 |= msg5_X10;
   if (configuration.isARCenabled()) request.ICMND.msg5 |= msg5_ARC;
   if (configuration.isACenabled()) request.ICMND.msg5 |= msg5_AC;
   if (configuration.isHEEUenabled()) request.ICMND.msg5 |= msg5_HEU;
   if (configuration.isMEIANTECHenabled()) request.ICMND.msg5 |= msg5_MEI;
   if (configuration.isOREGONenabled()) request.ICMND.msg5 |= msg5_OREGON;
   if (configuration.isATIenabled()) request.ICMND.msg5 |= msg5_ATI;
   if (configuration.isVISONICenabled()) request.ICMND.msg5 |= msg5_VISONIC;
   request.ICMND.msg6 = 0;
   if (configuration.isKeeLoqenabled()) request.ICMND.msg6 |= msg6_KeeLoq;
   if (configuration.isHomeConfortenabled()) request.ICMND.msg6 |= msg6_HC;

   return toBuffer(request, GET_RBUF_STRUCT_SIZE(ICMND));
}

shared::communication::CByteBuffer CTransceiver::buildStartReceiverCmd() const
{
   RBUF request;
   MEMCLEAR(request.ICMND); // For better performance, just clear the needed sub-structure of RBUF

   request.ICMND.packetlength = ENCODE_PACKET_LENGTH(ICMND);
   request.ICMND.packettype = pTypeInterfaceControl;
   request.ICMND.subtype = sTypeInterfaceCommand;
   request.ICMND.seqnbr = m_seqNumberProvider->next();
   request.ICMND.cmnd = cmdStartRec;

   return toBuffer(request, GET_RBUF_STRUCT_SIZE(ICMND));
}


boost::shared_ptr<std::queue<shared::communication::CByteBuffer>> CTransceiver::buildMessageToDevice(boost::shared_ptr<yApi::IYPluginApi> api,
                                                                                                     boost::shared_ptr<const yApi::IDeviceCommand> command) const
{
   try
   {
      const auto& deviceDetails = api->getDeviceDetails(command->getDevice());
      auto deviceType = deviceDetails.get<unsigned char>("type");

      // Create the RFXCom message
      switch (deviceType)
      {
      case pTypeLighting1:
         return rfxcomMessages::CLighting1(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeLighting2:
         return rfxcomMessages::CLighting2(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeLighting3:
         return rfxcomMessages::CLighting3(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeLighting4:
         return rfxcomMessages::CLighting4(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeLighting5:
         return rfxcomMessages::CLighting5(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeLighting6:
         return rfxcomMessages::CLighting6(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeChime:
         return rfxcomMessages::CChime(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeCurtain:
         return rfxcomMessages::CCurtain1(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeFan:
         return rfxcomMessages::CFan(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeBlinds:
         return rfxcomMessages::CBlinds1(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeRFY:
         return rfxcomMessages::CRfy(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeHomeConfort:
         return rfxcomMessages::CHomeConfort(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeSecurity1:
         return rfxcomMessages::CSecurity1(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeSecurity2:
         return rfxcomMessages::CSecurity2(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeCamera:
         return rfxcomMessages::CCamera1(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeRemote:
         return rfxcomMessages::CRemote(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeThermostat1:
         return rfxcomMessages::CThermostat1(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeThermostat2:
         return rfxcomMessages::CThermostat2(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeThermostat3:
         return rfxcomMessages::CThermostat3(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeThermostat4:
         return rfxcomMessages::CThermostat4(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeRadiator1:
         return rfxcomMessages::CRadiator1(api, command->getKeyword(), command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      case pTypeFS20:
         return rfxcomMessages::CFS20(api, command->getBody(), deviceDetails).encode(m_seqNumberProvider);
      default:
         YADOMS_LOG(error) << "Invalid command \"" << command->getBody() << "\" : " << " unknown type " << deviceType;
         throw shared::exception::CInvalidParameter(command->getBody());
      }
   }
   catch (shared::exception::CException& e)
   {
      std::string message = (boost::format("Invalid command \"%1%\" : %2%") % command->getBody() % e.what()).str();
      throw shared::exception::CInvalidParameter(message);
   }
}

boost::shared_ptr<rfxcomMessages::IRfxcomMessage> CTransceiver::decodeRfxcomMessage(boost::shared_ptr<yApi::IYPluginApi> api,
                                                                                    const shared::communication::CByteBuffer& data) const
{
   try
   {
      const auto buf = reinterpret_cast<const RBUF* const>(data.begin());
      const auto bufSize = data.size();

      boost::shared_ptr<rfxcomMessages::IRfxcomMessage> message;
      switch (buf->RXRESPONSE.packettype)
      {
      case pTypeInterfaceMessage: message = boost::make_shared<rfxcomMessages::CTransceiverStatus>(*buf, bufSize, m_seqNumberProvider);
         break;
      case pTypeRecXmitMessage: message = boost::make_shared<rfxcomMessages::CAck>(*buf, bufSize, m_seqNumberProvider);
         break;
      case pTypeRFXMeter: message = boost::make_shared<rfxcomMessages::CRFXMeter>(api, *buf, bufSize);
         break;
      case pTypeLighting1: message = boost::make_shared<rfxcomMessages::CLighting1>(api, *buf, bufSize);
         break;
      case pTypeLighting2: message = boost::make_shared<rfxcomMessages::CLighting2>(api, *buf, bufSize);
         break;
      case pTypeLighting3: message = boost::make_shared<rfxcomMessages::CLighting3>(api, *buf, bufSize);
         break;
      case pTypeLighting4: message = boost::make_shared<rfxcomMessages::CLighting4>(api, *buf, bufSize);
         break;
      case pTypeLighting5: message = boost::make_shared<rfxcomMessages::CLighting5>(api, *buf, bufSize);
         break;
      case pTypeLighting6: message = boost::make_shared<rfxcomMessages::CLighting6>(api, *buf, bufSize);
         break;
      case pTypeChime: message = boost::make_shared<rfxcomMessages::CChime>(api, *buf, bufSize);
         break;
      case pTypeFan: message = boost::make_shared<rfxcomMessages::CFan>(api, *buf, bufSize);
         break;
      case pTypeCurtain: message = boost::make_shared<rfxcomMessages::CCurtain1>(api, *buf, bufSize);
         break;
      case pTypeBlinds: message = boost::make_shared<rfxcomMessages::CBlinds1>(api, *buf, bufSize);
         break;
      case pTypeRFY: message = boost::make_shared<rfxcomMessages::CRfy>(api, *buf, bufSize);
         break;
      case pTypeHomeConfort: message = boost::make_shared<rfxcomMessages::CHomeConfort>(api, *buf, bufSize);
         break;
      case pTypeTEMP_RAIN: message = boost::make_shared<rfxcomMessages::CTempRain>(api, *buf, bufSize);
         break;
      case pTypeTEMP: message = boost::make_shared<rfxcomMessages::CTemp>(api, *buf, bufSize);
         break;
      case pTypeHUM: message = boost::make_shared<rfxcomMessages::CHumidity>(api, *buf, bufSize);
         break;
      case pTypeTEMP_HUM: message = boost::make_shared<rfxcomMessages::CTempHumidity>(api, *buf, bufSize);
         break;
      case pTypeBARO: message = boost::make_shared<rfxcomMessages::CBarometric>(api, *buf, bufSize);
         break;
      case pTypeTEMP_HUM_BARO: message = boost::make_shared<rfxcomMessages::CTempHumidityBarometric>(api, *buf, bufSize);
         break;
      case pTypeRAIN: message = boost::make_shared<rfxcomMessages::CRain>(api, *buf, bufSize);
         break;
      case pTypeWIND: message = boost::make_shared<rfxcomMessages::CWind>(api, *buf, bufSize);
         break;
      case pTypeUV: message = boost::make_shared<rfxcomMessages::CUV>(api, *buf, bufSize);
         break;
      case pTypeDT: message = boost::make_shared<rfxcomMessages::CDateTime>(api, *buf, bufSize);
         break;
      case pTypeCURRENT: message = boost::make_shared<rfxcomMessages::CCurrent>(api, *buf, bufSize);
         break;
      case pTypeENERGY: message = boost::make_shared<rfxcomMessages::CEnergy>(api, *buf, bufSize);
         break;
      case pTypeCURRENTENERGY: message = boost::make_shared<rfxcomMessages::CCurrentEnergy>(api, *buf, bufSize, m_unsecuredProtocolFilters.at(pTypeCURRENTENERGY));
         break;
      case pTypePOWER: message = boost::make_shared<rfxcomMessages::CPower>(api, *buf, bufSize);
         break;
      case pTypeWEIGHT: message = boost::make_shared<rfxcomMessages::CWeight>(api, *buf, bufSize);
         break;
      case pTypeCARTELECTRONIC: message = boost::make_shared<rfxcomMessages::CCartelectronic>(api, *buf, bufSize);
         break;
      case pTypeRFXSensor: message = boost::make_shared<rfxcomMessages::CRFXSensor>(api, *buf, bufSize);
         break;
      case pTypeSecurity1: message = boost::make_shared<rfxcomMessages::CSecurity1>(api, *buf, bufSize);
         break;
      case pTypeSecurity2: message = boost::make_shared<rfxcomMessages::CSecurity2>(api, *buf, bufSize);
         break;
      case pTypeCamera: message = boost::make_shared<rfxcomMessages::CCamera1>(api, *buf, bufSize);
         break;
      case pTypeRemote: message = boost::make_shared<rfxcomMessages::CRemote>(api, *buf, bufSize);
         break;
      case pTypeThermostat1: message = boost::make_shared<rfxcomMessages::CThermostat1>(api, *buf, bufSize);
         break;
      case pTypeThermostat2: message = boost::make_shared<rfxcomMessages::CThermostat2>(api, *buf, bufSize);
         break;
      case pTypeThermostat3: message = boost::make_shared<rfxcomMessages::CThermostat3>(api, *buf, bufSize);
         break;
      case pTypeRadiator1: message = boost::make_shared<rfxcomMessages::CRadiator1>(api, *buf, bufSize);
         break;
      case pTypeBBQ: message = boost::make_shared<rfxcomMessages::CBbq>(api, *buf, bufSize);
         break;
      case pTypeFS20: message = boost::make_shared<rfxcomMessages::CFS20>(api, *buf, bufSize);
         break;
      default:
         {
            YADOMS_LOG(error) << "Invalid RfxCom message received, unknown packet type " << std::setfill('0')
               << std::setw(sizeof(unsigned char) * 2) << std::hex << static_cast<int>(buf->RXRESPONSE.packettype);
            break;
         }
      }
      return message;
   }
   catch (std::exception& exception)
   {
      YADOMS_LOG(error) << "Invalid RfxCom message received : " << exception.what();
      return boost::shared_ptr<rfxcomMessages::IRfxcomMessage>();
   }
}

std::string CTransceiver::createDeviceManually(boost::shared_ptr<yApi::IYPluginApi> api,
                                               const yApi::IManuallyDeviceCreationData& data) const
{
   boost::shared_ptr<rfxcomMessages::IRfxcomMessage> msg;
   try
   {
      const auto activeSection = data.getConfiguration().get<std::string>("type.activeSection");
      
      // Lighting1
      if (activeSection == "x10")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeX10, data.getConfiguration().get<shared::CDataContainer>("type.content.x10.content"));
      else if (activeSection == "arc")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeARC, data.getConfiguration().get<shared::CDataContainer>("type.content.arc.content"));
      else if (activeSection == "ab400d")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeAB400D, data.getConfiguration().get<shared::CDataContainer>("type.content.ab400d.content"));
      else if (activeSection == "waveman")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeWaveman, data.getConfiguration().get<shared::CDataContainer>("type.content.waveman.content"));
      else if (activeSection == "emw200")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeEMW200, data.getConfiguration().get<shared::CDataContainer>("type.content.emw200.content"));
      else if (activeSection == "impuls")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeIMPULS, data.getConfiguration().get<shared::CDataContainer>("type.content.impuls.content"));
      else if (activeSection == "risingSun")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeRisingSun, data.getConfiguration().get<shared::CDataContainer>("type.content.risingSun.content"));
      else if (activeSection == "philips")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypePhilips, data.getConfiguration().get<shared::CDataContainer>("type.content.philips.content"));
      else if (activeSection == "energenie")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeEnergenie, data.getConfiguration().get<shared::CDataContainer>("type.content.energenie.content"));
      else if (activeSection == "energenie5")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeEnergenie5, data.getConfiguration().get<shared::CDataContainer>("type.content.energenie5.content"));
      else if (activeSection == "gdr2")
         msg = boost::make_shared<rfxcomMessages::CLighting1>(api, sTypeGDR2, data.getConfiguration().get<shared::CDataContainer>("type.content.gdr2.content"));

      // Lighting2
      else if (activeSection == "ac")
         msg = boost::make_shared<rfxcomMessages::CLighting2>(api, sTypeAC, data.getConfiguration().get<shared::CDataContainer>("type.content.ac.content"));
      else if (activeSection == "homeEasyEU")
         msg = boost::make_shared<rfxcomMessages::CLighting2>(api, sTypeHEU, data.getConfiguration().get<shared::CDataContainer>("type.content.homeEasyEU.content"));
      else if (activeSection == "anslut")
         msg = boost::make_shared<rfxcomMessages::CLighting2>(api, sTypeANSLUT, data.getConfiguration().get<shared::CDataContainer>("type.content.anslut.content"));
      else if (activeSection == "kambrookRf3672")
         msg = boost::make_shared<rfxcomMessages::CLighting2>(api, sTypeKambrook, data.getConfiguration().get<shared::CDataContainer>("type.content.kambrookRf3672.content"));

      // Lighting3
      else if (activeSection == "koppla")
         msg = boost::make_shared<rfxcomMessages::CLighting3>(api, sTypeKoppla, data.getConfiguration().get<shared::CDataContainer>("type.content.koppla.content"));

      // Lighting4
      else if (activeSection == "pt2262")
         msg = boost::make_shared<rfxcomMessages::CLighting4>(api, sTypePT2262, data.getConfiguration().get<shared::CDataContainer>("type.content.pt2262.content"));

      // Lighting5
      else if (activeSection == "lightwaveRf")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeLightwaveRF, data.getConfiguration().get<shared::CDataContainer>("type.content.lightwaveRf.content"));
      else if (activeSection == "emw100")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeEMW100, data.getConfiguration().get<shared::CDataContainer>("type.content.emw100.content"));
      else if (activeSection == "bbsb")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeBBSB, data.getConfiguration().get<shared::CDataContainer>("type.content.bbsb.content"));
      else if (activeSection == "mdRemote106")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeMDREMOTE, data.getConfiguration().get<shared::CDataContainer>("type.content.mdRemote106.content"));
      else if (activeSection == "rsl")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeRSL, data.getConfiguration().get<shared::CDataContainer>("type.content.rsl.content"));
      else if (activeSection == "livolo")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeLivolo, data.getConfiguration().get<shared::CDataContainer>("type.content.livolo.content"));
      else if (activeSection == "trc02")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeTRC02, data.getConfiguration().get<shared::CDataContainer>("type.content.trc02.content"));
      else if (activeSection == "aoke")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeAoke, data.getConfiguration().get<shared::CDataContainer>("type.content.aoke.content"));
      else if (activeSection == "trc02_2")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeTRC02_2, data.getConfiguration().get<shared::CDataContainer>("type.content.trc02_2.content"));
      else if (activeSection == "eurodomest")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeEurodomest, data.getConfiguration().get<shared::CDataContainer>("type.content.eurodomest.content"));
      else if (activeSection == "livoloAppliance")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeLivoloAppliance, data.getConfiguration().get<shared::CDataContainer>("type.content.livoloAppliance.content"));
      else if (activeSection == "rgb432w")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeRGB432W, data.getConfiguration().get<shared::CDataContainer>("type.content.rgb432w.content"));
      else if (activeSection == "mdremote107")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeMDREMOTE107, data.getConfiguration().get<shared::CDataContainer>("type.content.mdremote107.content"));
      else if (activeSection == "legrandCad")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeLegrandCAD, data.getConfiguration().get<shared::CDataContainer>("type.content.legrandCad.content"));
      else if (activeSection == "mdRemote108")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeMDREMOTE108, data.getConfiguration().get<shared::CDataContainer>("type.content.mdRemote108.content"));
      else if (activeSection == "kangtai")
         msg = boost::make_shared<rfxcomMessages::CLighting5>(api, sTypeKangtai, data.getConfiguration().get<shared::CDataContainer>("type.content.kangtai.content"));

      // Lighting6
      else if (activeSection == "blyss")
         msg = boost::make_shared<rfxcomMessages::CLighting6>(api, sTypeBlyss, data.getConfiguration().get<shared::CDataContainer>("type.content.blyss.content"));

      // Chime
      else if (activeSection == "byronSx")
         msg = boost::make_shared<rfxcomMessages::CChime>(api, sTypeByronSX, data.getConfiguration().get<shared::CDataContainer>("type.content.byronSx.content"));
      else if (activeSection == "byronMp001")
         msg = boost::make_shared<rfxcomMessages::CChime>(api, sTypeByronMP001, data.getConfiguration().get<shared::CDataContainer>("type.content.byronMp001.content"));
      else if (activeSection == "selectPlus")
         msg = boost::make_shared<rfxcomMessages::CChime>(api, sTypeSelectPlus, data.getConfiguration().get<shared::CDataContainer>("type.content.selectPlus.content"));
      else if (activeSection == "envivo")
         msg = boost::make_shared<rfxcomMessages::CChime>(api, sTypeEnvivo, data.getConfiguration().get<shared::CDataContainer>("type.content.envivo.content"));

      // Fan
      else if (activeSection == "siemensSf01")
         msg = boost::make_shared<rfxcomMessages::CFan>(api, sTypeSiemensSF01, data.getConfiguration().get<shared::CDataContainer>("type.content.siemensSf01.content"));
      else if (activeSection == "lucciAir")
         msg = boost::make_shared<rfxcomMessages::CFan>(api, sTypeLucciAir, data.getConfiguration().get<shared::CDataContainer>("type.content.lucciAir.content"));
      else if (activeSection == "seavTxs4")
         msg = boost::make_shared<rfxcomMessages::CFan>(api, sTypeSeavTXS4, data.getConfiguration().get<shared::CDataContainer>("type.content.seavTxs4.content"));
      else if (activeSection == "westinghouse7226640")
         msg = boost::make_shared<rfxcomMessages::CFan>(api, sTypeWestinghouse, data.getConfiguration().get<shared::CDataContainer>("type.content.westinghouse7226640.content"));

      // Curtain1
      else if (activeSection == "harrisonCurtain")
         msg = boost::make_shared<rfxcomMessages::CCurtain1>(api, sTypeHarrison, data.getConfiguration().get<shared::CDataContainer>("type.content.harrisonCurtain.content"));

      // Blinds1
      else if (activeSection == "rollerTrolHastaNew")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT0, data.getConfiguration().get<shared::CDataContainer>("type.content.rollerTrolHastaNew.content"));
      else if (activeSection == "hastaOld")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT1, data.getConfiguration().get<shared::CDataContainer>("type.content.hastaOld.content"));
      else if (activeSection == "aOkRf01")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT2, data.getConfiguration().get<shared::CDataContainer>("type.content.aOkRf01.content"));
      else if (activeSection == "aOkAc114")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT3, data.getConfiguration().get<shared::CDataContainer>("type.content.aOkAc114.content"));
      else if (activeSection == "raex")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT4, data.getConfiguration().get<shared::CDataContainer>("type.content.raex.content"));
      else if (activeSection == "mediaMount")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT5, data.getConfiguration().get<shared::CDataContainer>("type.content.mediaMount.content"));
      else if (activeSection == "dc106")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT6, data.getConfiguration().get<shared::CDataContainer>("type.content.dc106.content"));
      else if (activeSection == "forest")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT7, data.getConfiguration().get<shared::CDataContainer>("type.content.forest.content"));
      else if (activeSection == "chamberlaincs4330cn")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT8, data.getConfiguration().get<shared::CDataContainer>("type.content.chamberlaincs4330cn.content"));
      else if (activeSection == "sunperyBtx")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT9, data.getConfiguration().get<shared::CDataContainer>("type.content.sunperyBtx.content"));
      else if (activeSection == "dolatDlm1")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT10, data.getConfiguration().get<shared::CDataContainer>("type.content.dolatDlm1.content"));
      else if (activeSection == "asp")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT11, data.getConfiguration().get<shared::CDataContainer>("type.content.asp.content"));
      else if (activeSection == "confexx")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT12, data.getConfiguration().get<shared::CDataContainer>("type.content.confexx.content"));
      else if (activeSection == "screenline")
         msg = boost::make_shared<rfxcomMessages::CBlinds1>(api, sTypeBlindsT13, data.getConfiguration().get<shared::CDataContainer>("type.content.screenline.content"));

      // Rfy
      else if (activeSection == "rfy")
         msg = boost::make_shared<rfxcomMessages::CRfy>(api, sTypeRFY, data.getConfiguration().get<shared::CDataContainer>("type.content.rfy.content"));
      else if (activeSection == "rfyExt")
         msg = boost::make_shared<rfxcomMessages::CRfy>(api, sTypeRFYext, data.getConfiguration().get<shared::CDataContainer>("type.content.rfyExt.content"));
      else if (activeSection == "asa")
         msg = boost::make_shared<rfxcomMessages::CRfy>(api, sTypeASA, data.getConfiguration().get<shared::CDataContainer>("type.content.asa.content"));

      // HomeConfort
      else if (activeSection == "homeConfort")
         msg = boost::make_shared<rfxcomMessages::CHomeConfort>(api, sTypeHomeConfortTEL010, data.getConfiguration().get<shared::CDataContainer>("type.content.homeConfort.content"));

      // Security1
      else if (activeSection == "x10SecurityR")
         msg = boost::make_shared<rfxcomMessages::CSecurity1>(api, sTypeSecX10R, data.getConfiguration().get<shared::CDataContainer>("type.content.x10SecurityR.content"));

      // Security2
      else if (activeSection == "keeLoq")
         msg = boost::make_shared<rfxcomMessages::CSecurity2>(api, sTypeSec2Classic, data.getConfiguration().get<shared::CDataContainer>("type.content.keeLoq.content"));

      // Camera1
      else if (activeSection == "cameraX10Ninja")
         msg = boost::make_shared<rfxcomMessages::CCamera1>(api, sTypeNinja, data.getConfiguration().get<shared::CDataContainer>("type.content.cameraX10Ninja.content"));

      // Thermostat1
      else if (activeSection == "digimax")
         msg = boost::make_shared<rfxcomMessages::CThermostat1>(api, sTypeDigimax, data.getConfiguration().get<shared::CDataContainer>("type.content.digimax.content"));
      else if (activeSection == "digimaxShort")
         msg = boost::make_shared<rfxcomMessages::CThermostat1>(api, sTypeDigimaxShort, data.getConfiguration().get<shared::CDataContainer>("type.content.digimaxShort.content"));

      // Thermostat2
      else if (activeSection == "he105")
         msg = boost::make_shared<rfxcomMessages::CThermostat2>(api, sTypeHE105, data.getConfiguration().get<shared::CDataContainer>("type.content.he105.content"));
      else if (activeSection == "rts10")
         msg = boost::make_shared<rfxcomMessages::CThermostat2>(api, sTypeRTS10, data.getConfiguration().get<shared::CDataContainer>("type.content.rts10.content"));

      // Thermostat3
      else if (activeSection == "g6rH4t1")
         msg = boost::make_shared<rfxcomMessages::CThermostat3>(api, sTypeMertikG6RH4T1, data.getConfiguration().get<shared::CDataContainer>("type.content.g6rH4t1.content"));
      else if (activeSection == "g6rH4tb")
         msg = boost::make_shared<rfxcomMessages::CThermostat3>(api, sTypeMertikG6RH4TB, data.getConfiguration().get<shared::CDataContainer>("type.content.g6rH4tb.content"));
      else if (activeSection == "g6rH4td")
         msg = boost::make_shared<rfxcomMessages::CThermostat3>(api, sTypeMertikG6RH4TD, data.getConfiguration().get<shared::CDataContainer>("type.content.g6rH4td.content"));
      else if (activeSection == "g6rH4s")
         msg = boost::make_shared<rfxcomMessages::CThermostat3>(api, sTypeMertikG6RH4S, data.getConfiguration().get<shared::CDataContainer>("type.content.g6rH4s.content"));

      // Thermostat4
      else if (activeSection == "mcz1PelletStove")
         msg = boost::make_shared<rfxcomMessages::CThermostat4>(api, sTypeMCZ1, data.getConfiguration().get<shared::CDataContainer>("type.content.mcz1PelletStove.content"));
      else if (activeSection == "mcz2PelletStove")
         msg = boost::make_shared<rfxcomMessages::CThermostat4>(api, sTypeMCZ2, data.getConfiguration().get<shared::CDataContainer>("type.content.mcz2PelletStove.content"));
      else if (activeSection == "mcz2PelletStove")
         msg = boost::make_shared<rfxcomMessages::CThermostat4>(api, sTypeMCZ3, data.getConfiguration().get<shared::CDataContainer>("type.content.mcz2PelletStove.content"));

      // Radiator1
      else if (activeSection == "smartwares")
         msg = boost::make_shared<rfxcomMessages::CRadiator1>(api, sTypeSmartwares, data.getConfiguration().get<shared::CDataContainer>("type.content.smartwares.content"));

      // FS20
      else if (activeSection == "fs20")
         msg = boost::make_shared<rfxcomMessages::CFS20>(api, sTypeFS20, data.getConfiguration().get<shared::CDataContainer>("type.content.fs20.content"));
      else if (activeSection == "fht8v")
         msg = boost::make_shared<rfxcomMessages::CFS20>(api, sTypeFHT8V, data.getConfiguration().get<shared::CDataContainer>("type.content.fht8v.content"));
      else if (activeSection == "fht80")
         msg = boost::make_shared<rfxcomMessages::CFS20>(api, sTypeFHT80, data.getConfiguration().get<shared::CDataContainer>("type.content.fht80.content"));

      else
         throw shared::exception::CInvalidParameter("Unknown device type");
   }
   catch (shared::exception::CInvalidParameter& e)
   {
      YADOMS_LOG(error) << "Fail to create device manually, invalid parameter : " << e.what();
      throw CManuallyDeviceCreationException("invalid parameter");
   }
   catch (shared::exception::COutOfRange& e)
   {
      YADOMS_LOG(error) << "Fail to create device manually, out of range : " << e.what();
      throw CManuallyDeviceCreationException("out of range");
   }

   return msg->getDeviceName();
}
