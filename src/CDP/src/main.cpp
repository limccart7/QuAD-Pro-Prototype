#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include "Packet.h"
#include "BloomFilter.h"
#include "Utils.h"
#include "DuckLink.h"
//#include "MamaDuck.h"
#include "PapaDuck.h"
//#include "DetectorDuck.h"
#include "redis.h"

using namespace std;

BloomFilter filter = BloomFilter(DEFAULT_NUM_SECTORS, DEFAULT_NUM_HASH_FUNCS, DEFAULT_BITS_PER_SECTOR, DEFAULT_MAX_MESSAGES);

vector<std::string> extractValues(const std::string input) {
    std::vector<std::string> values;

    
    std::stringstream ss(input);
    std::string token;

    
    std::string duid, topic, data, ducktype;
    bool inDataField = false;

    while (std::getline(ss, token, '_')) {
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            std::string fieldName = token.substr(0, colonPos);
            std::string fieldValue = token.substr(colonPos + 1);

            if (fieldName == "DUID") {
                duid = fieldValue;
            } else if (fieldName == "TOPIC") {
                topic = fieldValue;
            } else if (fieldName == "DATA") {
                // Start collecting DATA field
                data = fieldValue;
                inDataField = true;
            } else if (fieldName == "DUCKTYPE") {
                // End of DATA field, collect DUCKTYPE field
                ducktype = fieldValue;
                inDataField = false;
            }
        } else if (inDataField) {
            // Append to DATA field if we are still in the DATA field
            data += ' ' + token;
        }
    }

    values.push_back(duid);
    values.push_back(topic);
    values.push_back(data);
    values.push_back(ducktype);

    return values;
}

string modifystring (string cdp, int position){
    if(cdp[position] > 127){//this goes into extended ascii range
        cdp[position] = cdp[position] ^ 0x80;
    }
    else if (cdp[position] < 32){//non printable characters
        cdp[position] = cdp[position] ^ 0x20;
    }
    else {
        //dont modify
    }
    
    
    return cdp;
}
string unmodifystring (string cdp, int position){
    
     if ((cdp[position] ^ 0x20) < 32){//non printable characters
        cdp[position] = cdp[position] ^ 0x20;
    }else if ((cdp[position] ^ 0x80) >127){//this goes into extended ascii range
        cdp[position] = cdp[position] ^ 0x80;
    }

    else {
        //dont modify
    }
    
    
    return cdp;
}

string sendToWeb (vector<uint8_t> receivedData) {

    //vector<uint8_t> receivedData;
    vector<uint8_t> sduid;
    vector<uint8_t> receivedMsg;
    string receivedSduid;
    string receivedTopic;
    string receivedMessage;
    string messageForWeb;

    receivedMsg.assign(receivedData.begin()+DATA_POS, receivedData.end());
    sduid.assign(receivedData.begin(), receivedData.begin() + 8);
    receivedSduid = duckutils::convertVectorToString(sduid);
    receivedTopic = Packet::topicToString(receivedData.at(TOPIC_POS));
    receivedMessage = duckutils::convertVectorToString(receivedMsg);
    messageForWeb = "SDUID:" + receivedSduid + "_TOPIC:"  + receivedTopic + "_DATA:" + receivedMessage +"_";

    return messageForWeb;

}

string encodeCDPAlex(string cdppayload) {
    for (int i = 0; i < 7; i++) {//modifies all unreadable characters within the string to make sure they are readable
        cdppayload = modifystring(cdppayload, TOPIC_POS + i);
    }
    return cdppayload;
}

string decodeCDPAlex(string cdppayload) {
    for (int i = 0; i < 7; i++) {//undoes modifying of string to get actual data initially put in
        cdppayload = unmodifystring(cdppayload, TOPIC_POS + i);
    }
    return cdppayload;
}
string decodeCDP(string inputString){

	cout << "In Decode Function" << endl;
	Packet dp;
	PapaDuck pd;

	cout << "input String: " << endl;
	cout << inputString << endl;

	inputString = decodeCDPAlex(inputString);
	cout << "Input String after CDPAlex" << endl;
	cout << inputString << endl;
	dp.setBuffer(duckutils::convertStringToVector(inputString));
	pd.handleReceivedPacket(dp);
	vector <uint8_t> receivedData = dp.getBuffer();

	string messageForWeb = sendToWeb(receivedData);
	return messageForWeb;
}

string encodeCDP(string web_stream ){

	//Function takes redis input stream from WEB side and returns formatted string
	

	//varibles which dont need to be stored
	
	cout << "In Encode Function" << endl;
	DuckLink dl;
	vector<uint8_t> dduid;
	uint8_t topic;
	vector<uint8_t> data;
	string DUCKTYPE;

	std::vector<std::string> extractedValues;
				
	extractedValues = extractValues(web_stream);
				
	cout << "DUID: " <<extractedValues[0] << endl;
	cout << "TOPIC: " << extractedValues[1] << endl;
	cout << "DATA: " << extractedValues[2] << endl;
	cout << "DUCKTYPE: " << extractedValues[3] << endl;


	dduid = duckutils::convertStringToVector(extractedValues[0]);
	topic = Packet::stringToTopic(extractedValues[1]);
	data = duckutils::convertStringToVector(extractedValues[2]);
	DUCKTYPE = extractedValues[3];

	// Instantiate Packet Object, form packet now

	dl.setDuckId(duckutils::convertStringToVector("DUCK0001")); //

	BloomFilter filter = BloomFilter(DEFAULT_NUM_SECTORS, DEFAULT_NUM_HASH_FUNCS, DEFAULT_BITS_PER_SECTOR, DEFAULT_MAX_MESSAGES);

	dl.prepareForSending(&filter, dduid, topic, dl.getType(), 0x00, data);

	/*---------Send to Lora---------*/
	vector<uint8_t> payload = dl.getBuffer();
	string cdppayload = duckutils::convertVectorToString(payload);

	cdppayload = encodeCDPAlex(cdppayload);
	
	cout << "printing formed packet\n" << endl;
	cout << cdppayload << endl;


	cout << "\n\n\n" << endl;

	return cdppayload;
}

                //publish(redisConnect, redisConfigLora.stream_name, "CDP_LORA", cdppayload, redisConfigLora.response);
                /*---------Send to Lora---------*/

                //dp.~Packet();



//RedisConfig initializeRedisWebConfig() {
    //RedisConfig config;
    //config.stream_name = "mystream";
    //config.group_name = "TX";
    //config.consumer_name = "CDP";
//config.filter_key = "WEB_CDP";
    //config.txKey = "WEB_CDP";
    //config.rxKey = "CDP_WEB";
    //config.txWebQueue = "TXWeb";
    //config.rxWebQueue = "RXWeb";
    //config.txLoraQueue;
    //config.response ;
    //config.task;
    //config.messageID;
    //config.key_buffer;
    //config.messageBuffer;
    //return config;
//}

//RedisConfig initializeRedisLoraConfig() {
    //RedisConfig config;
    //config.stream_name = "mystream";
    //config.group_name = "TX";
    //config.consumer_name = "CDP";
    //config.filter_key = "CDP_LORA";
    //config.txKey = "CDP_LORA";
    //config.rxKey = "LORA_CDP";
    //config.rxLoraQueue = "RXLora";
    ////config.txWebQueue;
    //config.txLoraQueue = "TXLora";
    //config.response ;
    //config.task;
    //config.messageID;
    //config.key_buffer;
    //config.messageBuffer;
    //return config;
//};

int main()
{

    
   

//////connect redis server

	redisContext* redisConnect = redis_init("localhost", 6379);
const string web_queue = "web_queue";
const string lora_queue = "lora_queue";

if (web_queue != lora_queue) {        
       	cout << "web_queue and lora_queue are different.\n";    
}
else {
       	cout << "web_queue and lora_queue are the same.\n";
}
    redisCommand(redisConnect, "DEL %s", web_queue.c_str());

    redisCommand(redisConnect, "DEL %s", lora_queue.c_str());


if (web_queue != lora_queue) {        
       	cout << "web_queue and lora_queue are different.\n";    
}
else {
       	cout << "web_queue and lora_queue are the same.\n";
}

string value = "DUID:MAMA0001 TOPIC:status DATA:Test Data String DUCKTYPE:LINK ";
string value2 = "DUID:MAMA0001 TOPIC:status DATA:Test Data String Again DUCKTYPE:PAPA ";
string value3 = "DUCK0001MAMA0001V16U0# R:weTest Data String Again";

create_consumer_group(redisConnect, "mystream", "CDP");

    /*------Duck objects----------*/
    DuckLink dl;
    //DetectorDuck dd;
    //MamaDuck md;
    PapaDuck pd;

    /*---------for WEB_CDP--------------*/
    std::vector<std::string> extractedValues;
    vector<uint8_t> dduid;
    uint8_t topic;
    vector<uint8_t> data;
    string DUCKTYPE;

    /*---------for CDP_WEB-------------*/
    vector<uint8_t> receivedData;
    vector<uint8_t> sduid;
    vector<uint8_t> receivedMsg;
    string receivedSduid;
    string receivedTopic;
    string receivedMessage;
    string messageForWeb;

    /*----------for CDP_Lora-----------*/
    vector<uint8_t> payload;
    string cdppayload;

			extractedValues = extractValues(value2);


			cout << "DUID: " <<extractedValues[0] << endl;
			cout << "TOPIC: " << extractedValues[1] << endl;
			cout << "DATA: " << extractedValues[2] << endl;
			cout << "DUCKTYPE: " << extractedValues[3] << endl;

			dduid = duckutils::convertStringToVector(extractedValues[0]);
			topic = Packet::stringToTopic(extractedValues[1]);
			data = duckutils::convertStringToVector(extractedValues[2]);
			DUCKTYPE = extractedValues[3];

			// Instantiate Packet Object
			Packet dp;
      
			//EXAMPLE: 
			//decoding web info to get ducktype
/*			
			string default_message = "DUID:DUCK0002_TOPIC:status_DATA:HELLO WORLD_DUCKTYPE:PAPADUCK";

			string cdp_formed_packet = encodeCDP(default_message);
			cout << "redis input from web:   " + default_message << endl;
			cout << "cdp formatted packet:   " + cdp_formed_packet << endl;
	
*/
			const string stream_name = "mystream";
			const string group_name = "CDP";
			const string consumer_name = "consumer";
			vector<KeyValuePair> messageArray;
			while(1)
			{
			    messageArray.clear();	
				read_from_consumer_group_dynamic(redisConnect, stream_name, 
						group_name, consumer_name, messageArray);
				
    			size_t numMessages = messageArray.size();
				
				for (uint8_t messageCounter = 0; messageCounter < numMessages; 
						messageCounter ++) {
					if (messageArray[messageCounter].key == "WEB_CDP") {
						enqueue_task(redisConnect, web_queue, 
								messageArray[messageCounter].value);
						acknowledge_message(redisConnect, stream_name, 
								group_name, messageArray[messageCounter].messageID); 
						
						check_pending_messages(redisConnect, stream_name, group_name);
					
					}

					else if (messageArray[messageCounter].key == "LORA_CDP") {
						cout << "LORA_CDP key Block" << endl;
					//	messageArray[messageCounter].value = decodeCDPAlex(messageArray[messageCounter].value);
						enqueue_task(redisConnect, lora_queue, 
								messageArray[messageCounter].value);

						acknowledge_message(redisConnect, stream_name, 
								group_name, messageArray[messageCounter].messageID);
					
						check_pending_messages(redisConnect, stream_name, group_name);
					}
					
					else {
						acknowledge_message(redisConnect, stream_name, 
								group_name, messageArray[messageCounter].messageID);
					}
					check_pending_messages(redisConnect, stream_name, group_name);
				}

				cout << "PreQueue Logic" << endl;
				print_queue(redisConnect, lora_queue);
				print_queue(redisConnect, web_queue);
				if (queue_len(redisConnect, web_queue) > 0) {
					cout << "WEB Queue" << endl;
					string task_buffer;
					dequeue_task(redisConnect, web_queue, task_buffer);
					string outputMessage = encodeCDP(task_buffer);
					publish(redisConnect, stream_name, "CDP_LORA", 
							outputMessage, task_buffer);
				}
				if (queue_len(redisConnect, lora_queue) > 0) {
					cout << "LORA Queue" << endl;
					string task_buffer;
					dequeue_task(redisConnect, lora_queue, task_buffer);
					string outputMessage = decodeCDP(task_buffer);
					publish(redisConnect, stream_name, "CDP_WEB", 
							outputMessage, task_buffer);
				}
				print_queue(redisConnect, web_queue);
				print_queue(redisConnect, lora_queue);


			sleep(5);
	}
}

