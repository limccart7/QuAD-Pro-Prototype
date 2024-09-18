#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include "redis.h"
int main(int argc, char *argv[]) {
	char messageBuff[100];

	//user has provided a message
	if (argc < 2) {
		printf("Usage: %s <message>\n", argv[0]);
		return 1;
	}

	// Copy the message to the buffer
    strncpy(messageBuff, argv[1], sizeof(messageBuff) - 1);
    messageBuff[sizeof(messageBuff) - 1] = '\0'; // Ensure null-termination
		

	redisContext * c = redis_init("127.0.0.1", 6379);
    char response[256]; // Ensure response buffer is large enough
    publish(c, "mystream", "WEB_CDP", messageBuff, response);

    // Clean up
    redisFree(c);
    return 0;
}


