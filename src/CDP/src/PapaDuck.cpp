#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include "Packet.h"
#include "PapaDuck.h"
#include "BloomFilter.h"
#include "Utils.h"
#include "redis.h"

#define DUCK_ERR_NONE 0

using std::cout;
using std::endl;



void PapaDuck::handleReceivedPacket(Packet &packet) {


  int err= 0;
  cout << "handleReceivedPacket() START" << endl;
  //declare string to receive packet

  

  
  packet.decodePacket(packet.getBuffer());
  //duckutils::printVector(dataRX);
  //int err = duckRadio.readReceivedData(&data);

  if (err != DUCK_ERR_NONE) {
    cout << "ERROR handleReceivedPacket. Failed to get data. rc = " << err << endl;
    return;
  }
  // ignore pings
  if (packet.getBuffer().at(TOPIC_POS) == reservedTopic::ping) {
    packet.reset();
    return;
  }
  


  // build our RX DuckPacket which holds the updated path in case the packet is relayed
  bool relay = packet.checkRelayPacket(&filter, packet.getBuffer());
  if (relay) {
    cout << "relaying: " <<  duckutils::convertToHex(packet.getBuffer().data(), packet.getBuffer().size()).c_str() << endl;
    
    
    //recvDataCallback(rxPacket->getBuffer());
    enableAcks(1);
    if (acksEnabled) {
      
      if (needsAck(packet)) {
        handleAck(packet);
      }
    
  }

    

    cout << "handleReceivedPacket() DONE" << endl;
  }
}

void PapaDuck::handleAck(Packet& packet) {
  /*if (ackTimer.empty()) {
    logdbg_ln("Starting new ack broadcast timer with a delay of %d ms", timerDelay);
    ackTimer.in(timerDelay, ackHandler, this);
  }*/
  cout << "Starting new ack broadcast" << endl;
  storeForAck(packet);

  if (ackBufferIsFull()) {
    cout << "Ack buffer is full. Sending broadcast ack immediately." << endl;
    broadcastAck();
  }
}

void PapaDuck::enableAcks(bool enable) {
  acksEnabled = enable;
}

bool PapaDuck::ackHandler(PapaDuck * duck)
{
  duck->broadcastAck();
  return false;
}

void PapaDuck::storeForAck(Packet & packet) {
  ackStore.push_back(std::pair<Duid, Muid>(packet.sduid, packet.muid));
}

bool PapaDuck::ackBufferIsFull() {
  return (ackStore.size() >= MAX_MUID_PER_ACK);
}

bool PapaDuck::needsAck(Packet & packet) {
  if (packet.getBuffer().at(TOPIC_POS) == reservedTopic::ack) {
    return false;
  } else {
    return true;
  }
}

void PapaDuck::broadcastAck() {
  assert(ackStore.size() <= MAX_MUID_PER_ACK);

  const uint8_t num = static_cast<uint8_t>(ackStore.size());

  std::vector<uint8_t> dataPayload;
  dataPayload.push_back(num);
  for (int i = 0; i < num; i++) {
    Duid duid = ackStore[i].first;
    Muid muid = ackStore[i].second;
    cout << "Storing ack for duid: " << duckutils::convertVectorToString(duid) <<  "muid: " << duckutils::convertVectorToString(muid) << endl;
      
      
    dataPayload.insert(dataPayload.end(), duid.begin(), duid.end());
    dataPayload.insert(dataPayload.end(), muid.begin(), muid.end());
  }
  Packet txAck;
  int err = txAck.prepareForSending(&filter, BROADCAST_DUID, DuckType::PAPA,
    reservedTopic::ack, 0, dataPayload);
  

  //err = duckRadio.sendData(txPacket->getBuffer());

  if (err == DUCK_ERR_NONE) {
    
    filter.bloom_add(txAck.muid.data(), MUID_LENGTH);
  } else {
    cout << "ERROR handleReceivedPacket. Failed to send ack. Error: " << err << endl;
  }
  
  ackStore.clear();
}

void PapaDuck::sendCommand(uint8_t cmd, std::vector<uint8_t> value) {
  cout << "Initiate sending command" << endl;
  std::vector<uint8_t> dataPayload;
  dataPayload.push_back(cmd);
  dataPayload.insert(dataPayload.end(), value.begin(), value.end());

  Packet txCommand;
  int err = txCommand.prepareForSending(&filter, BROADCAST_DUID, DuckType::PAPA,
    reservedTopic::cmd, 0, dataPayload);
  if (err != DUCK_ERR_NONE) {
    
    cout << "ERROR handleReceivedPacket. Failed to prepare ack. Error: " << err << endl;
  }

  //err = duckRadio.sendData(txPacket->getBuffer());

  if (err == DUCK_ERR_NONE) {
    //Packet packet = Packet(txPacket->getBuffer());

    filter.bloom_add(txCommand.muid.data(), MUID_LENGTH);
  } else {
    cout << "ERROR handleReceivedPacket. Failed to send ack. Error: " << err << endl;
  }
  
}

void PapaDuck::sendCommand(uint8_t cmd, std::vector<uint8_t> value, std::vector<uint8_t> dduid) {
  //loginfo_ln("Initiate sending command");
  std::vector<uint8_t> dataPayload;
  dataPayload.push_back(cmd);
  dataPayload.insert(dataPayload.end(), value.begin(), value.end());

  Packet txCommand;
  int err = txCommand.prepareForSending(&filter, dduid, DuckType::PAPA,
    reservedTopic::cmd, 0, dataPayload);
  if (err != DUCK_ERR_NONE) {
    cout << "ERROR handleReceivedPacket. Failed to prepare cmd. Error: " << endl;
  }

  //err = duckRadio.sendData(txPacket->getBuffer());

  if (err == DUCK_ERR_NONE) {
    //Packet packet = Packet(txPacket->getBuffer());
    
    filter.bloom_add(txCommand.muid.data(), MUID_LENGTH);
  } else {
    cout << "ERROR handleReceivedPacket. Failed to send cmd. Error: " << err << endl;
  }
}

string unmodifyString (string cdp, int position){
    if(cdp[position] > 54){
        cdp[position] = cdp[position] - 55;
    }
    else{
        cdp[position] = cdp[position] - 48;
    }
    cdp[position] = cdp[position] - 32;
    return cdp;
}

/*int main()
{

////set device ID
    string devID = "MAMA0003";

    // Instantiate Packet Object
    Packet dp;

    PapaDuck pd;

    pd.setDuckId(duckutils::convertStringToVector("PAPA0003"));

    vector<uint8_t> packetRecieved;
    //packetRecieved = {'P', 'A', 'P', 'A' , 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A' , 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A' , 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A', 'P', 'A'};
    string CDP = "DUCK0001MAMA0003Y4D5G50��mTest Data String";
                  DUCK0001MAMA0003DSIYS|6Test Data String

    for(int i = 0; i < 7; i++)
    {
        CDP = unmodifyString(CDP, TOPIC_POS+i);
    }

    packetRecieved = duckutils::convertStringToVector(CDP);
    dp.setBuffer(packetRecieved);

    //gets payload generated
    vector<uint8_t> payload = dp.getBuffer();

    //Packet packet = Packet(dp.getBuffer());
    //duckutils::printVector(packet.getBuffer());

  
    pd.handleReceivedPacket(dp);

    //dp.reset();

    /*string destination = CDP.substr(DDUID_POS, devID.length());

    //check if this is the packet is going to this device
    if(destination == devID)
    {
        //TODO handle duck commands, acknowledgements, and pings


        //parses sections of received cdp payload
        vector<string> outputFileData;
        outputFileData = dp.decodePacket(payload);

        CDP = duckutils::convertVectorToString(payload);

        cout << "CDP packet as a string: " << CDP << endl;

        writeFileWebServer("outfile.txt", outputFileData);
    }
    else if (destination  == duckutils::convertVectorToString(BROADCAST_DUID))
    {

    }
    else
    {
        //Handle relaying data
        cout << "Not this duck" << endl;
    }
    //if not this do nothing

    return 0;
}*/