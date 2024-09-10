#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>

void publish(redisContext *redisConnect, const char *stream_name, const char *key, const char *value, char *response) {
    if (stream_name == NULL || key == NULL || value == NULL || response == NULL) {
        return;
    }

    // Calculate the required command length
    size_t command_len = 8 + strlen(stream_name) + 1 + strlen(key) + 1 + strlen(value) + 1; // "XADD " + stream name + " * " + key + value + null terminator

    // Allocate memory for the command
    char *command = malloc(command_len * sizeof(char));
    if (!command) {
        printf("Memory allocation error\n");
        return;
    }

    // Build the command string
    snprintf(command, command_len, "XADD %s * %s %s", stream_name, key, value);

    // Send the command to Redis
    redisReply *reply = (redisReply *)redisCommand(redisConnect, command);

    if (reply == NULL) {
        printf("Command execution error\n");
        free(command);
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        printf("ERROR: %s\n", reply->str);
    } else if (reply->type == REDIS_REPLY_STRING || reply->type == REDIS_REPLY_STATUS) {
        strcpy(response, reply->str);
        printf("Message added with ID: %s\n", reply->str);
    } else if (reply->type == REDIS_REPLY_NIL) {
        printf("No response from Redis\n");
    } else {
        printf("Unexpected reply type: %d\n", reply->type);
    }

    // Free the reply and command memory
    freeReplyObject(reply);
    free(command);
}


redisContext * redis_init(char * server, int port){
	puts("redis init");
	redisContext * c = redisConnect(server, port);
	if ( c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
		}
		else {
			printf("Conenction error:can't allocate redis context\n");
		}
		exit(1);
	}
	puts("connection established");	
	puts("end message");
	return c;
}
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
int main() {
    // Connect to Redis
	//
	
    redisContext *c = redis_init("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Can't allocate redis context\n");
        }
        exit(1);
    }

    char response[256]; // Ensure response buffer is large enough
    publish(c, "mystream", "key1", "valuetobegeneratedinCDPpacket", response);

    // Clean up
    redisFree(c);
    return 0;
}

