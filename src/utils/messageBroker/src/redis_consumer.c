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

	//creating an array of key value pairs to be populated when reading stream
	int maxMessages = 100;
	size_t totalMessages = 0;


	KeyValuePair * messageArray = malloc(maxMessages * sizeof(KeyValuePair));
    if (messageArray == NULL) {
		fprintf(stderr, "Mem Allocation failed\n");
		exit(1);
	}

	read_from_consumer_group_dynamic(c, stream_name, consumer_group, consumer_name, &messageArray, &totalMessages);

	//print the messages
	for (int messageCounter = 0; messageCounter < totalMessages; messageCounter++) {
		printf("Key Received: %s\n", messageArray[messageCounter].key);
		printf("Message Received: %s\n", messageArray[messageCounter].key);
		printf("Message ID Received: %s\n", messageArray[messageCounter].messageID);

		check_pending_messages(c,stream_name,consumer_group, messageArray[messageCounter].messageID);
		acknowledge_message(c, stream_name, consumer_group, messageArray[messageCounter].messageID);
	}
	redisFree(c);
	return 0;	
}


