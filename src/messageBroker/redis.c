#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include "redis.h"
#include <unistd.h>  // For sleep
redisContext * redis_init(const char * server, int port){
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
void publish(redisContext *redisConnect, const char *stream_name, const char *key, const char *value, char *response) {
    if (stream_name == NULL || key == NULL || value == NULL || response == NULL) {
        return;
    }

    // Send the command to Redis using argument-based function
    redisReply *reply = (redisReply *)redisCommand(redisConnect, "XADD %s * %s %s", stream_name, key, value);

    if (reply == NULL) {
        printf("Command execution error\n");
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

    // Free the reply object
    freeReplyObject(reply);
}
void read_from_consumer_group_dynamic(redisContext *c, const char *stream_name, const char *group_name, const char *consumer_name, KeyValuePair **messageArray, size_t *totalMessages) {
    redisReply *reply = (redisReply *)redisCommand(c, "XREADGROUP GROUP %s %s STREAMS %s >", group_name, consumer_name, stream_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        *messageArray = NULL;
        *totalMessages = 0;
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        *totalMessages = 0;

        // Calculate the total number of messages to allocate memory accordingly
        for (size_t i = 0; i < reply->elements; i++) {
            redisReply *stream = reply->element[i];
            redisReply *messages = stream->element[1];
            *totalMessages += messages->elements;
        }

        *messageArray = malloc(*totalMessages * sizeof(KeyValuePair));
        if (*messageArray == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            freeReplyObject(reply);
            *totalMessages = 0;
            return;
        }

        size_t messageIndex = 0;

        for (size_t i = 0; i < reply->elements; i++) {
            redisReply *stream = reply->element[i];
            redisReply *messages = stream->element[1];

            for (size_t j = 0; j < messages->elements; j++) {
                redisReply *message = messages->element[j];
                printf("Message ID: %s\n", message->element[0]->str);
                (*messageArray)[messageIndex].messageID = strdup(message->element[0]->str);

                redisReply *fields = message->element[1];

                for (size_t k = 0; k < fields->elements; k += 2) {
                    // Allocate memory for key and value
                    (*messageArray)[messageIndex].key = strdup(fields->element[k]->str);
                    (*messageArray)[messageIndex].value = strdup(fields->element[k + 1]->str);

                    if ((*messageArray)[messageIndex].key == NULL || (*messageArray)[messageIndex].value == NULL) {
                        fprintf(stderr, "Memory allocation failed\n");
                        // Free previously allocated memory before returning
                        for (size_t m = 0; m < messageIndex; m++) {
                            free((*messageArray)[m].key);
                            free((*messageArray)[m].value);
                        }
                        free(*messageArray);
                        freeReplyObject(reply);
                        *messageArray = NULL;
                        *totalMessages = 0;
                        return;
                    }

                    printf("Storing message #%zu - Key: %s, Value: %s, messageID: %s \n", messageIndex, (*messageArray)[messageIndex].key, (*messageArray)[messageIndex].value, (*messageArray)[messageIndex].messageID);
                    messageIndex++;
                }

            }
        }
    } else {
        printf("No messages found.\n");
        *messageArray = NULL;
        *totalMessages = 0;
    }
	puts("end of dyanmic function");
    freeReplyObject(reply);
}

/*
void read_from_consumer_group(redisContext *c, const char *stream_name, const char *group_name, const char *consumer_name, char * key_buffer, char *message_buffer, char *messageID) {
	redisReply *reply = (redisReply *)redisCommand(c, "XREADGROUP GROUP %s %s STREAMS %s >", group_name, consumer_name, stream_name);

	if (reply == NULL) {
		printf("Command execution error\n");
		return;
	}

	if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
		for (size_t i = 0; i < reply->elements; i++) {
			redisReply *stream = reply->element[i];
			redisReply *messages = stream->element[1];

			for (size_t j = 0; j < messages->elements; j++) {
				redisReply *message = messages->element[j];
				printf("Message ID: %s\n", message->element[0]->str);  // Message ID (if you need it)
				strcat(messageID, message->element[0]->str);
				redisReply *fields = message->element[1];
				message_buffer[0] = '\0'; // Clear the buffer before concatenating the message

				// Extract and concatenate key-value pairs into the message buffer
				for (size_t k = 0; k < fields->elements; k += 2) {
					printf("field element 2, should be message buffer%s", 
							fields->element[k+1]->str);
					strcat(key_buffer, fields->element[k]->str);
					strcat(message_buffer, fields->element[k + 1]->str);
					printf("catting to buffers : %d\n", (k/2));
					printf("key buffer : %s\n", key_buffer);
					printf("mes buffer : %s\n", message_buffer);
					if (k + 2 < fields->elements) {
						strcat(message_buffer, ", ");  // Separate key-value pairs with a comma
					}
				}

			}
		}
	} else {
		printf("No messages found.\n");
		memset(key_buffer, 0, 100);
		memset(message_buffer, 0,100); 
	}

	freeReplyObject(reply);
}
*/

void readStream(redisContext *redis_connect, const char *stream_name, char *response) {
    if (stream_name == NULL || response == NULL) {
        return;
    }

    // Initialize response buffer
    response[0] = '\0';  // Ensure the response buffer starts empty

    redisReply *reply = (redisReply *)redisCommand(redis_connect, "XRANGE %s - +", stream_name);

    if (reply->type == REDIS_REPLY_ARRAY) { 
        for (size_t i = 0; i < reply->elements; i++) {
            redisReply *message = reply->element[i];
            redisReply *fields = message->element[1];

            // Assuming the message is stored as key-value pairs, and you want the value (not the key)
            for (size_t j = 1; j < fields->elements; j += 2) {
				printf("stream key : %s\n",  fields->element[j-1]->str);
				printf("reading stream : %s\n", fields->element[j]->str);
                strcat(response, fields->element[j]->str); // Append the value to the response buffer

                if (j + 2 < fields->elements) {
                    strcat(response, ", ");  // Add a comma and space between multiple values
                }
            }
        }
    }
    freeReplyObject(reply);
}

void create_consumer_group(redisContext *c, const char *stream_name, const char *group_name) {
    redisReply *reply = (redisReply *)redisCommand(c, "XGROUP CREATE %s %s $ MKSTREAM", stream_name, group_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        printf("ERROR: %s\n", reply->str);
    } else {
        printf("Consumer group %s created on stream %s\n", group_name, stream_name);
    }

    freeReplyObject(reply);
}
void acknowledge_message(redisContext *c, const char *stream_name, const char *group_name, const char *message_id) {
    redisReply *reply = (redisReply *)redisCommand(c, "XACK %s %s %s", stream_name, group_name, message_id);

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_INTEGER) {
        printf("Acknowledged %lld messages\n", reply->integer);
    } else {
        printf("Failed to acknowledge message\n");
    }

    freeReplyObject(reply);
}
void check_pending_messages(redisContext *c, const char *stream_name, const char *group_name, char * messageID) {
    redisReply *reply = (redisReply *)redisCommand(c, "XPENDING %s %s", stream_name, group_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 4) {
        printf("Pending count: %lld\n", reply->element[0]->integer);
      
		/*printf("Smallest Pending ID: %s\n", reply->element[1]->str);*/
		/*if (reply->element[1]->str != NULL) {*/
		/*strcat(messageID, reply->element[1]->str);*/
		/*}*/
		/*printf("message passed to buffer %s\n", messageID);*/
        /*printf("Largest Pending ID: %s\n", reply->element[2]->str);*/
        /*printf("Consumers with pending messages: %llu\n", reply->element[3]->elements);*/
    } else {
        printf("No pending messages.\n");
    }

    freeReplyObject(reply);
}

int retrieve_message(redisContext *redisConnect, const char *stream_name, const char *group_name, const char *message_id, char *key, char *value) {
    if (stream_name == NULL || group_name == NULL || message_id == NULL || key == NULL || value == NULL) {
        return -1; // Error: Invalid input
    }

    // Debugging output
    printf("message id passed into retrieve_message: %s\n", message_id);

    // Send the command to Redis to retrieve the message by ID
    redisReply *reply = (redisReply *)redisCommand(redisConnect, "XREADGROUP GROUP %s myconsumer COUNT 1 STREAMS %s %s", group_name, stream_name, message_id);

    if (reply == NULL) {
        printf("Command execution error\n");
        return -1; // Error: Redis command failed
    }

    // Debugging output
    printf("Message ID passed to XREADGROUP: %s\n", message_id);

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        redisReply *stream = reply->element[0];
        redisReply *messages = stream->element[1];

        if (messages->type == REDIS_REPLY_ARRAY && messages->elements > 0) {
            redisReply *message = messages->element[0];
            redisReply *fields = message->element[1];

            if (fields->type == REDIS_REPLY_ARRAY && fields->elements >= 2) {
                // Assuming key and value are in pairs
                strcpy(key, fields->element[0]->str);
                strcpy(value, fields->element[1]->str);

                // Debugging output
                printf("Retrieved key: %s, value: %s\n", key, value);

                // Free the reply object
                freeReplyObject(reply);
                return 0; // Success
            }
        }
    } else {
        // Additional diagnostic output
        printf("Unexpected reply type or structure: type=%d, elements=%lu\n", reply->type, reply->elements);
    }

    // Handle cases where the message or key-value pairs were not found
    printf("No message found with ID: %s\n", message_id);

    // Free the reply object
    freeReplyObject(reply);
    return -1; // Error: Message not found
}
int retrieve_smallest_pending(redisContext *redisConnect, const char *stream_name, const char *group_name, char *key, char *value) {
    if (stream_name == NULL || group_name == NULL || key == NULL || value == NULL) {
        return -1; // Error: Invalid input
    }

    // Step 1: Get the smallest pending message ID
    redisReply *pending_reply = (redisReply *)redisCommand(redisConnect, "XPENDING %s %s - + 1", stream_name, group_name);

    if (pending_reply == NULL || pending_reply->type != REDIS_REPLY_ARRAY || pending_reply->elements < 4) {
        printf("No pending messages found or command execution error\n");
        if (pending_reply) freeReplyObject(pending_reply);
        return -1; // Error: No pending messages or command failed
    }

    const char *message_id = pending_reply->element[0]->str;

    // Step 2: Read the message by its ID
    redisReply *message_reply = (redisReply *)redisCommand(redisConnect, "XREADGROUP GROUP %s myconsumer COUNT 1 STREAMS %s %s", group_name, stream_name, message_id);

    if (message_reply == NULL || message_reply->type != REDIS_REPLY_ARRAY || message_reply->elements == 0) {
        printf("Failed to retrieve the message by ID: %s\n", message_id);
        if (message_reply) freeReplyObject(message_reply);
        freeReplyObject(pending_reply);
        return -1; // Error: Message not found or command failed
    }

    redisReply *stream = message_reply->element[0];
    redisReply *messages = stream->element[1];

    if (messages->type == REDIS_REPLY_ARRAY && messages->elements > 0) {
        redisReply *message = messages->element[0];
        redisReply *fields = message->element[1];

        if (fields->type == REDIS_REPLY_ARRAY && fields->elements >= 2) {
            // Assuming key and value are in pairs
            strcpy(key, fields->element[0]->str);
            strcpy(value, fields->element[1]->str);

            // Free the reply objects
            freeReplyObject(message_reply);
            freeReplyObject(pending_reply);
            return 0; // Success
        }
    }

    // Handle cases where the key-value pairs were not found
    printf("No key-value pairs found for message ID: %s\n", message_id);

    // Free the reply objects
    freeReplyObject(message_reply);
    freeReplyObject(pending_reply);
    return -1; // Error: Key-value pairs not found
}
void delete_stream(redisContext * c, const char *stream_name){

	redisReply *reply = (redisReply *)redisCommand(c, "DEL %s", stream_name);

if (reply == NULL) {
    printf("Command execution error\n");
} else if (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
    printf("Stream '%s' deleted successfully.\n", stream_name);
} else {
    printf("Stream '%s' does not exist or could not be deleted.\n", stream_name);
}

freeReplyObject(reply);
}
void enqueue_task(redisContext *c, const char *queue_name, const char *task) {
    redisReply *reply = (redisReply *)redisCommand(c, "RPUSH %s %s", queue_name, task);
    if (reply->type == REDIS_REPLY_INTEGER) {
        printf("\n\n\nTask added to queue '%s'. Queue length: %lld\n\n\n", queue_name, reply->integer);
    }
    freeReplyObject(reply);
}

void dequeue_task(redisContext *c, const char *queue_name, char * taskBuffer) {
    redisReply *reply = (redisReply *)redisCommand(c, "LPOP %s", queue_name);
    if (reply->type == REDIS_REPLY_STRING) {
        printf("Dequeued task: %s\n\n\n", reply->str);
		strcat(taskBuffer, reply->str);
    } else {
        printf("Queue is empty or command failed.\n");
    }
    freeReplyObject(reply);
}
void print_queue(redisContext *c, const char *queue_name) {
    // Fetch all elements from the queue using LRANGE 0 -1
    redisReply *reply = (redisReply *)redisCommand(c, "LRANGE %s 0 -1", queue_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        printf("Queue '%s' contains %zu elements:\n", queue_name, reply->elements);
        for (size_t i = 0; i < reply->elements; i++) {
            printf("%zu: %s\n", i + 1, reply->element[i]->str);
        }
    } else {
        printf("Queue '%s' is empty or command failed.\n", queue_name);
    }

    freeReplyObject(reply);
}

int queue_len(redisContext *c, const char *queue_name) {
    // Execute the LLEN command to get the length of the list
    redisReply *reply = (redisReply *)redisCommand(c, "LLEN %s", queue_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        return -1;  // Return -1 to indicate an error
    }

    int length = 0;

    if (reply->type == REDIS_REPLY_INTEGER) {
        length = (int)reply->integer;  // The length of the queue
    } else {
        printf("Unexpected reply type: %d\n", reply->type);
    }

    freeReplyObject(reply);
    return length;
}

int get_and_process_first_pending_message(redisContext *c, const char *stream_name, const char *group_name, const char *consumer_name, char *keyBuffer, char *messageBuffer, char *messageIDBuffer) {
    // Fetch information about pending messages
    redisReply *reply = (redisReply *)redisCommand(c, "XPENDING %s %s - + 1", stream_name, group_name);

    if (reply == NULL) {
        printf("Command execution error\n");
        return -1;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0 && reply->element[0]->elements > 0) {
        // Get the smallest pending ID
        const char *smallest_pending_id = reply->element[0]->element[0]->str;
        /*printf("Smallest Pending ID: %s\n", smallest_pending_id);*/

        // Copy the smallest pending ID to the messageIDBuffer
        strncpy(messageIDBuffer, smallest_pending_id, 100);  // Adjust size as needed
        messageIDBuffer[99] = '\0'; // Ensure null termination

        // Read the message with the smallest pending ID using XREADGROUP without acknowledging it
        redisReply *message_reply = (redisReply *)redisCommand(c, "XREADGROUP GROUP %s %s COUNT 1 STREAMS %s %s", group_name, consumer_name, stream_name, smallest_pending_id);

        if (message_reply == NULL) {
            printf("Failed to read the pending message.\n");
            freeReplyObject(reply);
            return -1;
        }

        if (message_reply->type == REDIS_REPLY_ARRAY && message_reply->elements > 0) {
            redisReply *message = message_reply->element[0]->element[1];  // The actual message data
            redisReply *fields = message->element[1];

            // Clear buffers
            memset(keyBuffer, 0, sizeof(keyBuffer));
            memset(messageBuffer, 0, sizeof(messageBuffer));

            // Extract key-value pairs
            for (size_t i = 0; i < fields->elements; i += 2) {
                if (i == 0) {
                    strcpy(keyBuffer, fields->element[i]->str);  // Copy the key
                }
                strcpy(messageBuffer, fields->element[i + 1]->str);  // Copy the value
            }

            printf("Message ID: %s\n", messageIDBuffer);
            printf("Key: %s\n", keyBuffer);
            printf("Message: %s\n", messageBuffer);

            freeReplyObject(message_reply);
        } else {
            printf("No message found with the smallest pending ID.\n");
            freeReplyObject(message_reply);
            freeReplyObject(reply);
            return -1;
        }

    } else {
        printf("No pending messages.\n");
        freeReplyObject(reply);
        return -1;
    }

    freeReplyObject(reply);
    return 0;
}
