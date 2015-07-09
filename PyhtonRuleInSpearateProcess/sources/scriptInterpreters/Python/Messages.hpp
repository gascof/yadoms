#pragma once


const size_t m_messageQueueMessageSize(1000);


//--------------------------------------------------------------
/// \brief Messages requests identifiers
//--------------------------------------------------------------
enum ERequestIdentifier
{
   kReqReadKeyword = 0,
   kReqWaitForAcquisition,
   //TODO compléter
};

//--------------------------------------------------------------
/// \brief Messages answers identifiers
//--------------------------------------------------------------
enum EAnswerIdentifier
{
   kAnsReadKeyword = 0,
   kAnsWaitForAcquisition,
   //TODO compléter
};

//--------------------------------------------------------------
/// \brief The read keyword request
//--------------------------------------------------------------
class CReqReadKeyword
{
public:
   CReqReadKeyword() {}
   virtual ~CReqReadKeyword() {}

   int m_keywordId;

private:
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive& ar, const unsigned int version)
   {
      ar & m_keywordId;
   }
};

//--------------------------------------------------------------
/// \brief The wait for acquisition request
//--------------------------------------------------------------
class CReqWaitForAcquisition
{
public:
   CReqWaitForAcquisition() {}
   virtual ~CReqWaitForAcquisition() {}

   int m_keywordId;
   std::string m_timeout;

private:
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive& ar, const unsigned int version)
   {
      ar & m_keywordId;
      ar & m_timeout;
   }
};

//--------------------------------------------------------------
/// \brief The read keyword answer
//--------------------------------------------------------------
struct CAnsReadKeyword
{
public:
   CAnsReadKeyword() {}
   virtual ~CAnsReadKeyword() {}

   std::string m_returnValue;

private:
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive& ar, const unsigned int version)
   {
      ar & m_returnValue;
   }
};

//--------------------------------------------------------------
/// \brief The wait for acquisition answer
//--------------------------------------------------------------
class CAnsWaitForAcquisition
{
public:
   CAnsWaitForAcquisition() {}
   virtual ~CAnsWaitForAcquisition() {}

   std::string m_returnValue;

private:
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive& ar, const unsigned int version)
   {
      ar & m_returnValue;
   }
};