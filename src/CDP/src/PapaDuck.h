
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <cassert>
#include "Packet.h"
//#include "MamaDuck.h"
#include "BloomFilter.h"
#include "Utils.h"
#include <utility>  // For std::pair

class PapaDuck : public Packet {
    public:

    using rxDoneCallback = void (*)(std::vector<uint8_t> data );
    using txDoneCallback = void (*)(void);
    ~PapaDuck() {
    }


    int getType() { return DuckType::PAPA; }
    void enableAcks(bool enable);
    void sendCommand(uint8_t cmd, std::vector<uint8_t> value);
    void sendCommand(uint8_t cmd, std::vector<uint8_t> value, std::vector<uint8_t> dduid);
    //void handleReceivedPacket();
    void handleReceivedPacket(Packet &packet);

    private: 

    
    void handleAck(Packet& packet);
    bool needsAck(Packet & packet);
    void storeForAck(Packet & packet);
    bool ackBufferIsFull();
    void broadcastAck();

    static bool ackHandler(PapaDuck * duck);
    typedef std::vector<uint8_t> Duid; 
    typedef std::vector<uint8_t>  Muid;
    typedef std::vector<std::pair<Duid, Muid> > AckStore;

    AckStore ackStore;
    bool acksEnabled{false};

    protected:
    BloomFilter filter;
};