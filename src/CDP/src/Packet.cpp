#include "Packet.h"
#include "Utils.h"
#include "BloomFilter.h"
#include "CRC32.h"
#include "redis.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring> // For memcpy


using namespace std;

#define DUCK_ERR_NONE 0
#define DUCKPACKET_ERR_SIZE_INVALID -4000
#define DUCKPACKET_ERR_TOPIC_INVALID -4001
#define DUCKPACKET_ERR_MAX_HOPS -4002
#define DUCK_ERR_ID_TOO_LONG -5101
#define DUCK_ERR_OTA -5200

void Packet::getMessageId(BloomFilter *filter, uint8_t message_id[MUID_LENGTH])
{
  bool getNewUnique = true;
  while (getNewUnique)
  {
    duckutils::getRandomBytes(MUID_LENGTH, message_id);
    getNewUnique = filter->bloom_check(message_id, MUID_LENGTH);
    cout << "prepareForSending: new MUID -> " << duckutils::convertToHex(message_id, MUID_LENGTH).c_str() << endl;
  }
}

int Packet::prepareForSending(BloomFilter *filter, vector<uint8_t> dduid, uint8_t topic, uint8_t duckType, uint8_t hopCount, vector<uint8_t> data)
{

  int data_length = data.size();

  this->reset();

  // Check data exists within correct size parameters
  if (data.empty())
  {
    cout << "Error: Duck Packet Empty";
    return DUCKPACKET_ERR_SIZE_INVALID;
  }
  else if (data_length > MAX_DATA_LENGTH)
  {
    cout << "Error: Duck Packet Too Large" << endl;
    return DUCKPACKET_ERR_SIZE_INVALID;
  }

  cout << "prepareForSending: DATA LENGTH: " << data_length << " - TOPIC (" << Packet::topicToString(topic).c_str() << ")" << endl;

  uint8_t message_id[MUID_LENGTH];
  getMessageId(filter, message_id);

  calculateCRC(data);

  // ----- insert packet header  -----

  // source device uid
  buffer.insert(buffer.end(), sduid.begin(), sduid.end());

  // destination device uid
  buffer.insert(buffer.end(), dduid.begin(), dduid.end());
  this->dduid = dduid;

  // message uid
  buffer.insert(buffer.end(), &message_id[0], &message_id[MUID_LENGTH]);
  this->muid = muid;

  // topic
  buffer.push_back(topic);
  this->topic = topic;
 
  // duckType
  buffer.push_back(duckType);
  this->duckType = duckType;

  // hop count
  buffer.push_back(hopCount);
  this->hopCount = hopCount;

  // data crc
  dataCRC = duckutils::convert32BitToVector(dcrc);
  buffer.insert(buffer.end(), dataCRC.begin(), dataCRC.end());
  this->dcrc = dcrc;
  
  // data
  //buffer.push_back(0x00);//for duck commands set to 0 for now
  buffer.insert(buffer.end(), data.begin(), data.end());

  // ----- print packet -----
  const string bufferString = duckutils::convertToHex(buffer.data(), buffer.size()).c_str();

  cout << "SDuid:        " << duckutils::convertToHex(sduid.data(), sduid.size()).c_str() << endl;
  cout << "DDuid:        " << duckutils::convertToHex(dduid.data(), dduid.size()).c_str() << endl;
  cout << "Muid:         " << bufferString.substr(MUID_POS * 2, (TOPIC_POS - MUID_POS) * 2) << endl;
  cout << "Topic:        " << bufferString.substr((TOPIC_POS) * 2, (DUCK_TYPE_POS - TOPIC_POS) * 2) << endl;
  cout << "Duck Type:    " << bufferString.substr((DUCK_TYPE_POS) * 2, (HOP_COUNT_POS - DUCK_TYPE_POS) * 2) << endl;
  cout << "Hop Count:    " << bufferString.substr((HOP_COUNT_POS) * 2, (DATA_CRC_POS - HOP_COUNT_POS) * 2) << endl;
  cout << "Data CRC:     " << bufferString.substr((DATA_CRC_POS) * 2, (DATA_POS - DATA_CRC_POS) * 2) << endl;
  cout << "Data:         " << bufferString.substr(DATA_POS * 2) << endl;
  cout << "Built packet: " << duckutils::convertToHex(buffer.data(), buffer.size()).c_str() << endl;

  

  return DUCK_ERR_NONE;
}

void Packet::sendToLora(redisContext *c, vector<uint8_t> dataTx){
/* ----------------send to Lora---------------*/



/* ----------------send to Lora---------------*/
}

vector<string> Packet::decodePacket(vector<uint8_t> cdpPayload){

  //auto start = std::chrono::system_clock::now();
  
  //vector<uint8_t> sduid;
  sduid = parseCDPPacket(SDUID_POS, DDUID_POS, cdpPayload);
  
  //vector<uint8_t> dduid;
  dduid = parseCDPPacket(DDUID_POS, MUID_POS, cdpPayload);
  
  //vector<uint8_t> muid;
  muid = parseCDPPacket(MUID_POS, TOPIC_POS, cdpPayload);
  //muid = cdpPayload.at(MUID_POS);
  
  //vector<uint8_t> topic;
  //topic = parseCDPPacket(TOPIC_POS, DUCK_TYPE_POS, cdpPayload);
  topic = cdpPayload.at(TOPIC_POS);
  
  vector<uint8_t> duckType;
  duckType = parseCDPPacket(DUCK_TYPE_POS, HOP_COUNT_POS, cdpPayload);
  
  //vector<uint8_t> hopCount;
  //hopCount = parseCDPPacket(HOP_COUNT_POS, DATA_CRC_POS, cdpPayload);
  hopCount = cdpPayload.at(HOP_COUNT_POS);
  
  //vector<uint8_t> dcrc;
  dcrc = duckutils::toUint32(parseCDPPacket(DATA_CRC_POS, DATA_POS, cdpPayload));
  
  //vector<uint8_t> data;
  data = parseCDPPacket(DATA_POS, cdpPayload.size(), cdpPayload);


  //TODO: update calculateCRC for data section to check if data is different from what was recieved
  


  const string bufferString = duckutils::convertToHex(buffer.data(), buffer.size()).c_str();

  cout << "Received SDuid:        " << duckutils::convertToHex(sduid.data(), sduid.size()).c_str() << endl;
  cout << "Received DDuid:        " << duckutils::convertToHex(dduid.data(), dduid.size()).c_str() << endl;
  cout << "Received Muid:         " << bufferString.substr(MUID_POS * 2, (TOPIC_POS - MUID_POS) * 2) << endl;
  cout << "Received Topic:        " << bufferString.substr((TOPIC_POS) * 2, (DUCK_TYPE_POS - TOPIC_POS) * 2) << endl;
  cout << "Received Duck Type:    " << bufferString.substr((DUCK_TYPE_POS) * 2, (HOP_COUNT_POS - DUCK_TYPE_POS) * 2) << endl;
  cout << "Received Hop Count:    " << bufferString.substr((HOP_COUNT_POS) * 2, (DATA_CRC_POS - HOP_COUNT_POS) * 2) << endl;
  cout << "Received Data CRC:     " << bufferString.substr((DATA_CRC_POS) * 2, (DATA_POS - DATA_CRC_POS) * 2) << endl;
  cout << "Received Data:         " << bufferString.substr(DATA_POS * 2) << endl;
  cout << "Received Built packet: " << duckutils::convertToHex(buffer.data(), buffer.size()).c_str() << endl;

  vector<string> processOutput(5);
  processOutput[0] = duckutils::convertVectorToString(dduid);
  processOutput[1] = Packet::topicToString(topic);
  processOutput[2] = duckutils::convertVectorToString(data);

  

 auto end = std::chrono::system_clock::now();
 time_t end_time = std::chrono::system_clock::to_time_t(end);
 std::tm* local_time = std::localtime(&end_time);
 
    std::ostringstream date_stream;
    std::ostringstream time_stream;

    // Format date as M/D/Y
    date_stream << std::put_time(local_time, "%m-%d-%Y");
    std::string date = date_stream.str();

    // Format time as HH:MM:SS
    time_stream << std::put_time(local_time, "%H:%M:%S");
    std::string time = time_stream.str();

    // Output the results
    std::cout << "Date: " << date << std::endl;
    std::cout << "Time: " << time << std::endl;
 //cout << "finished computation at " << ctime(&end_time);
  processOutput[3] =date;
  processOutput[4] =time;


 return processOutput;

}

vector<uint8_t> Packet::parseCDPPacket (uint8_t startPosition, uint8_t endPosition,vector<uint8_t> payload)
{
  vector<uint8_t> parsedVec;
    for (uint8_t i = startPosition; i < endPosition; i++) {
        parsedVec.push_back(payload.at(i));
    }
    return parsedVec;
}

void Packet::setDuckId(vector<uint8_t> duckId)
{
  sduid = duckId;
}

void Packet::setBuffer(vector<uint8_t> packet)
{
  buffer = packet;
}

void Packet::calculateCRC(vector<uint8_t> data)
{
  std::vector<uint8_t> encryptedData;
  uint32_t value;
  value = crc32<IEEE8023_CRC32_POLYNOMIAL>(0xFFFFFFFF, data.begin(), data.end());
  dcrc = value;
}

bool Packet::checkRelayPacket(BloomFilter *filter, std::vector<uint8_t> dataBuffer) {
  unsigned char* buff = dataBuffer.data();
  bool alreadySeen = filter->bloom_check(&buff[MUID_POS], MUID_LENGTH);
  if(alreadySeen)
  {
    std::cout << "Packet has already been seen. No relay" << std::endl;
    return false;
  }
  else {
    //stores MUID into 
    filter->bloom_add(&dataBuffer[MUID_POS], MUID_LENGTH);
    //add statement print the packet
  }
  dataBuffer.at(HOP_COUNT_POS)++;
  buffer = dataBuffer;
  return true;
}

int Packet::sendPing(){
  int err = DUCK_ERR_NONE;
  std::vector<uint8_t> dataPing(1, 0);
  err = txPacket->prepareForSending(&filter, ZERO_DUID, this->getType(), reservedTopic::ping, txPacket->hopCount, dataPing);
  if (err != DUCK_ERR_NONE) {
    cout << "ERROR Failed to build ping packet, err = " << err << endl;
    return err;
  }

  //send to radio


  //err = duckRadio.sendData(txPacket->getBuffer());
  /*if (err != DUCK_ERR_NONE) {
    logerr_ln("ERROR Lora sendData failed, err = %d", err);
  }*/
  return err;
}


int Packet::sendPong(){
  int err = DUCK_ERR_NONE;
  vector<uint8_t> dataPong(1, 0);
  err = txPacket->prepareForSending(&filter, ZERO_DUID, this->getType(), reservedTopic::pong,txPacket->hopCount, dataPong);
  if (err != DUCK_ERR_NONE) {
    cout << "ERROR Oops! failed to build pong packet, err = " << err << endl;
    //return err;
  }
  /*err = duckRadio.sendData(txPacket->getBuffer());
  if (err != DUCK_ERR_NONE) {
    logerr_ln("ERROR Oops! Lora sendData failed, err = %d", err);
    return err;
  }*/
  return err;
}

