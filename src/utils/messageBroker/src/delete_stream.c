#include <stdio.h>
#include "redis.h"


// very simple program to erase the stored messages within the stream
int main(){

	redisContext * c = redis_init("127.0.0.1", 6379);
	delete_stream(c, "mystream");
	return 0;
}


