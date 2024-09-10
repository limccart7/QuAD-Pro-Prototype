#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <string.h>
void publish(redisContext * redisConnect, const char * stream_name, const char * message,  char * response);
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

    // Add messages to a stream
    const char *stream_name = "mystream";
    const char * message = "hello world how are you?";
	char response[100];
	publish(c, "mystream", "hello world how are you", response);

	/*redisReply *reply = (redisReply *)redisCommand(c, "XADD %s * key1 hello key2 world2", stream_name);*/

    /*if (reply->type == REDIS_REPLY_ERROR) {*/
        /*printf("Error: %s\n", reply->str);*/
    /*} else {*/
        /*printf("Message added with ID: %s\n", reply->str);*/
    /*}*/
    /*freeReplyObject(reply);*/

	 /*Clean up*/
    redisFree(c);
    return 0;
}
void publish(redisContext *redisConnect, const char *stream_name, const char *message, char *response) {
    if (stream_name == NULL || message == NULL || response == NULL) {
        return;
    }

    // Calculate the required command length
    size_t command_len = 8 + strlen(stream_name) + 1 + strlen(message) + 1; // "XADD " + stream name + " * " + message + null terminator

    // Allocate memory for the command
    char *command = malloc(command_len * sizeof(char));
    if (!command) {
        printf("Memory allocation error\n");
        return;
    }

    // Build the command string
    snprintf(command, command_len, "XADD %s * %s", stream_name, message);

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

/*void publish(redisContext * redisConnect, const char * stream_name, const char * message,  char * response){*/

	/*if (stream_name == NULL){*/
		/*return;*/
	/*}*/

	/*//redis has strict format for XADD so we need to build string for it with its format*/
	/*//*/
	/*//"XADD" + "stream_name" + "*" + "message"*/
	/*size_t command_len = 8 + strlen(stream_name) + 1; // "XADD " + stream name + " *" + null terminator*/
	/*command_len +=strlen(message);*/

	/*char * command = malloc(command_len * sizeof(char));*/
	/*if (!command) {*/
		/*printf("memory allocation error\n");*/
	/*}*/

	/*snprintf(command, command_len, "XADD %s *", stream_name);*/

	/*strcat(command, " ");*/
	/*strcat(command, message);*/



	/*redisReply * reply = (redisReply *)redisCommand(redisConnect, command);*/

	/*if (reply->type == REDIS_REPLY_ERROR){*/
		/*printf("ERROR %s\n", reply->str);*/
	/*}*/
	/*else {*/
		/*strcpy(reply->str, response);*/
		/*printf("Message added with ID: %s\n", reply->str);*/
	/*}*/

	/*freeReplyObject(reply);*/
/*}*/
