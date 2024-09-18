#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include "redis.h"


int main(){

	redisContext * c = redis_init("127.0.0.1", 6379);
	const char *stream_name = "mystream";
	const char *consumer_group = "group1";
	const char *consumer_name = "consumer_name";

	create_consumer_group(c, stream_name, consumer_group);
	
	char publish_response[100];

	char message_buffer[512];
	char messageID[30];	
    // Read messages from the consumer group
	puts("reading the stream from consumer group");
    readStream(c, stream_name, message_buffer);
/*
	read_from_consumer_group(c, stream_name, consumer_group, consumer_name, message_buffer, messageID);
	puts("printing response buffer:\n");
	printf("Message Content: %s\n", message_buffer);	
	printf("Message ID: %s\n", messageID);

	check_pending_messages(c, stream_name, consumer_group);
	puts("try to ack");
	acknowledge_message(c, stream_name, consumer_group, messageID);
	check_pending_messages(c, stream_name, consumer_group);
 */	
	redisFree(c);
}


