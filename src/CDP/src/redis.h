#ifndef REDIS_H
#define REDIS_H
#include <hiredis/hiredis.h>
#include <string>
#include <vector>
typedef struct {
	std::string key;
	std::string value;
	std::string messageID;
} KeyValuePair;  
using std::string;

redisContext * redis_init(const char * server, int port);
void publish(redisContext* redisConnect, const std::string& stream_name, const std::string& key, const std::string& value, std::string& response);
void read_from_consumer_group_dynamic(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, std::vector<KeyValuePair>& messageArray);
void readStream(redisContext* redis_connect, const std::string& stream_name, std::string& response);

void read_from_consumer_group(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, const std::string& filter_key, std::string& key_buffer, std::string& messageBuffer, std::string& messageID, std::string& queue_name, std::string& task);

std::string read_first_message_with_key(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, const std::string& filter_key, std::string& messageID );
//void create_consumer_group(redisContext * c, const char *stream_name, const char *group_name);

//void read_from_consumer_group(redisContext * c, const char *stream_name, const char *group_name, const char *consumer_name, char * key_buffer, char * message_buffer, char * messageID);
//void read_from_consumer_group(redisContext *c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, const std::string& filter_key, std::string& key_buffer, std::string& message_buffer, std::string& messageID);

void create_consumer_group(redisContext* c, const std::string& stream_name, const std::string& group_name);


int acknowledge_message(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& message_id) ;

void check_pending_messages(redisContext* c, const std::string& stream_name, const std::string& group_name);

//int get_and_process_first_pending_message(redisContext * c, const char * stream_name, const char *group_name, const char * consumer_name, char * keyBuffer, char * messageBuffer, char * messageIDBuffer);
void delete_stream(redisContext* c, const std::string& stream_name);

void enqueue_task(redisContext* c, const std::string& queue_name, const std::string& task);

void dequeue_task(redisContext* c, const std::string& queue_name, std::string& taskBuffer);

void print_queue(redisContext* c, const std::string& queue_name);

int queue_len(redisContext* c, const std::string& queue_name) ;

struct RedisConfig {
    string stream_name;
    string group_name;
    string consumer_name;
    string txKey;
    string rxKey;
    string txWebQueue;
    string txLoraQueue;
    string rxWebQueue;
    string rxLoraQueue;
    string response;
    string task;
    string messageID;
    string key_buffer;
    string messageBuffer;
};

/*struct RedisRxLoraConfig {
    string stream_name;
    string group_name;
    string consumer_name;
    string filter_key;
    string key;
    //string lora_queue;
    //string txWebQueue;
    //string txLoraQueue;
    string response;
    string task;
    string messageID;
    string key_buffer;
    string messageBuffer;
};*/

#endif
