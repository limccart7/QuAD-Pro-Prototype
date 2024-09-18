#include <hiredis/hiredis.h>
#include "redis.h"


int main(){

	redisContext * c = redis_init("127.0.0.1", 6379);
	const char *stream_name = "mystream";

	char publish_response[100];

	char message_buffer[512];
	char messageID[30];	

	readStream(c, stream_name, message_buffer);
	redisFree(c);
	return 0;	
}


