#ifndef REDIS_H
#define REDIS_H
#include <hiredis/hiredis.h>

typedef struct{
	char * key;
	char * value;
	char * messageID;
} KeyValuePair;

redisContext * redis_init(const char * server, int port);
void publish(redisContext *redisConnect, const char *stream_name, const char *key, const char *value, char *response);

void readStream(redisContext * redis_connect, const char * stream_name, char * response);


void create_consumer_group(redisContext * c, const char *stream_name, const char *group_name);

void read_from_consumer_group(redisContext * c, const char *stream_name, const char *group_name, const char *consumer_name, char * key_buffer, char * message_buffer, char * messageID);
void read_from_consumer_group_dynamic(redisContext *c, const char *stream_name, const char *group_name, const char *consumer_name, KeyValuePair **messageArray, size_t *totalMessages);
void acknowledge_message(redisContext * c, const char *stream_name, const char *group_name, const char *message_id);

void check_pending_messages(redisContext * c, const char *stream_name, const char *group_name, char * messageID);

void delete_stream(redisContext * c, const char *stream_name);

void enqueue_task(redisContext * c, const char * queue_name, const char *task);

void dequeue_task(redisContext * c, const char * queue_name, char * taskBuffer);

void print_queue(redisContext * c, const char * queue_name);

int queue_len(redisContext *c, const char * queue_name);




#endif


