#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
void readStream(redisContext * redis_connect, const char * stream_name, char * response);

int main() {
    // Connect to Redis
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Can't allocate redis context\n");
        }
        exit(1);
    }

	char streamMessage[100];

	readStream(c, "mystream", streamMessage);


    // Read messages from the stream
    /*const char *stream_name = "mystream";*/
    /*redisReply *reply = (redisReply *)redisCommand(c, "XRANGE %s - +", stream_name);*/

    /*if (reply->type == REDIS_REPLY_ARRAY) {*/
        /*for (size_t i = 0; i < reply->elements; i++) {*/
            /*redisReply *message = reply->element[i];*/
            /*printf("Message ID: %s\n", message->element[0]->str);*/

            /*redisReply *fields = message->element[1];*/
            /*for (size_t j = 0; j < fields->elements; j += 2) {*/
                /*printf("%s: %s\n", fields->element[j]->str, fields->element[j + 1]->str);*/
            /*}*/
        /*}*/
    /*}*/
    /*freeReplyObject(reply);*/


    // Clean up
    redisFree(c);
    return 0;
}
// Want to write a function which checks stream and fills pointer with value of respon

void readStream(redisContext * redis_connect, const char * stream_name, char * response) {

	if (stream_name == NULL) {
		return;
	}

	redisReply * reply = (redisReply *)redisCommand(redis_connect,
			"XRANGE %s - +", stream_name);
	
	if (reply->type == REDIS_REPLY_ARRAY) { //if the response is of certain type
		for (size_t i = 0; i < reply->elements; i++ ){
			redisReply *message = reply->element[i];
			printf("Message ID: %s\n", message->element[0]->str);
			redisReply *fields = message->element[1];

			for (size_t j = 0; j < fields->elements; j += 2){
				printf("%s: %s\n", fields->element[j]->str, fields->element[j+1]->str);
			}
		}
	}
	freeReplyObject(reply);
}

