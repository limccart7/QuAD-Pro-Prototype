#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <hiredis/hiredis.h>
#include "redis.h"
#include <vector>
void read_from_consumer_group_dynamic(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, std::vector<KeyValuePair>& messageArray) {
    redisReply* reply = (redisReply*)redisCommand(c, "XREADGROUP GROUP %s %s STREAMS %s >", group_name.c_str(), consumer_name.c_str(), stream_name.c_str());

    if (reply == NULL) {
        std::cerr << "Command execution error" << std::endl;
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        // Clear the message array before storing new messages
        messageArray.clear();

        for (size_t i = 0; i < reply->elements; i++) {
            redisReply* stream = reply->element[i];
            redisReply* messages = stream->element[1];

            for (size_t j = 0; j < messages->elements; j++) {
                redisReply* message = messages->element[j];
                std::string messageID = message->element[0]->str;
                redisReply* fields = message->element[1];

                for (size_t k = 0; k < fields->elements; k += 2) {
                    // Create a KeyValuePair and populate it
                    KeyValuePair kvp;
                    kvp.messageID = messageID;
                    kvp.key = fields->element[k]->str;
                    kvp.value = fields->element[k + 1]->str;

                    // Store the KeyValuePair in the message array
                    messageArray.push_back(kvp);

                    std::cout << "Storing message #" << messageArray.size() - 1
                              << " - Key: " << kvp.key
                              << ", Value: " << kvp.value
                              << ", messageID: " << kvp.messageID << std::endl;
                }
            }
        }
    } else {
        std::cout << "No messages found." << std::endl;
    }

    std::cout << "End of dynamic function" << std::endl;
    freeReplyObject(reply);
}


void enqueue_task(redisContext* c, const std::string& queue_name, const std::string& task);

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
void publish(redisContext* redisConnect, const std::string& stream_name, const std::string& key, const std::string& value, std::string& response) {
    if (stream_name.empty() || key.empty() || value.empty()) {
        return;
    }

    // Send the command to Redis
    redisReply* reply = (redisReply*)redisCommand(redisConnect, "XADD %s * %s %s", stream_name.c_str(), key.c_str(), value.c_str());

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        printf("ERROR: %s\n", reply->str);
    } else if (reply->type == REDIS_REPLY_STRING || reply->type == REDIS_REPLY_STATUS) {
        response = reply->str;
        printf("Message added with ID: %s\n", reply->str);
    } else if (reply->type == REDIS_REPLY_NIL) {
        printf("No response from Redis\n");
    } else {
        printf("Unexpected reply type: %d\n", reply->type);
    }

    // Free the reply object
    freeReplyObject(reply);
}
/*
void read_from_consumer_group(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, const std::string& filter_key, std::string& key_buffer, std::string& messageBuffer, std::string& messageID, std::string& queue_name, std::string& task) {
    // Execute the XREADGROUP command
    redisReply* reply = (redisReply*)redisCommand(c, "XREADGROUP GROUP %s %s STREAMS %s >", group_name.c_str(), consumer_name.c_str(), stream_name.c_str());

    if (reply == NULL) {
        std::cerr << "Command execution error: " << c->errstr << "\n";
        return;
    }

    // Clear buffers
    key_buffer.clear();
    messageBuffer.clear();
    messageID.clear();

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        for (size_t i = 0; i < reply->elements; i++) {
            redisReply* stream = reply->element[i];
            redisReply* messages = stream->element[1];

            for (size_t j = 0; j < messages->elements; j++) {
                redisReply* message = messages->element[j];
                messageID = message->element[0]->str; // Set the message ID

                redisReply* fields = message->element[1];
                bool found_key = false;

                // Extract and check key-value pairs
                for (size_t k = 0; k < fields->elements; k += 2) {
                    if (filter_key == fields->element[k]->str) {
                        // Key matches the filter_key
                        found_key = true;
                        key_buffer = fields->element[k]->str;
                        messageBuffer = fields->element[k + 1]->str;
                        task = messageBuffer;
                        std::cout << "Filtered Message Content: " << fields->element[k]->str << ": " << fields->element[k + 1]->str << "\n";
                        enqueue_task(c, queue_name, task);
                        task.clear();
                        
                    }
                }

                if (!found_key) {
                    // If no matching key found, clear buffers
                    key_buffer.clear();
                    messageBuffer.clear();
                }
            }
        }
    } else {
        std::cout << "No messages found.\n";
        // Clear buffers
        key_buffer.clear();
        messageBuffer.clear();
    }

    freeReplyObject(reply);
}
*/
std::string read_first_message_with_key(redisContext* c, const std::string& stream_name, const std::string& group_name, const std::string& consumer_name, const std::string& filter_key,std::string& messageID ) {
    // Format the command with strings
    std::string command = "XREADGROUP GROUP " + group_name + " " + consumer_name + " STREAMS " + stream_name + " >";

    // Execute the command
    redisReply* reply = (redisReply*)redisCommand(c, command.c_str());
    std::string result;

    if (reply == NULL) {
        std::cerr << "Command execution error\n";
        return result;  // Return an empty string if there's an error
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        for (size_t i = 0; i < reply->elements; i++) {
            redisReply* stream = reply->element[i];
            redisReply* messages = stream->element[1];

            for (size_t j = 0; j < messages->elements; j++) {
                redisReply* message = messages->element[j];
                messageID = message->element[0]->str;  // Extract Message ID

                redisReply* fields = message->element[1];
                bool found_key = false;

                // Extract and check key-value pairs
                for (size_t k = 0; k < fields->elements; k += 2) {
                    std::string key = fields->element[k]->str;
                    std::string value = fields->element[k + 1]->str;

                    if (key == filter_key) {
                        // Key matches the filter_key
                        found_key = true;
                        result = value;  // Value
                        std::cout << "Filtered Message Content: " << key << ": " << value << std::endl;

                        // Return after finding the first matching key
                        freeReplyObject(reply);
                        return result;
                    }
                }

                // If key is not found in this message, continue to the next message
                if (found_key) {
                    break; // Stop looking in the current message
                }
            }

            // If key is found, no need to look further
            if (!result.empty()) {
                break; // Stop looking in other messages
            }
        }
    } else {
        std::cout << "No messages found.\n";
    }

    freeReplyObject(reply);
    return result;  // Return the result which might be empty if no matching key was found
}

void readStream(redisContext* redis_connect, const std::string& stream_name, std::string& response) {
    if (stream_name.empty()) {
        return;
    }

    // Initialize response buffer
    response.clear();  // Ensure the response buffer starts empty

    redisReply* reply = (redisReply*)redisCommand(redis_connect, "XRANGE %s - +", stream_name.c_str());

    if (reply->type == REDIS_REPLY_ARRAY) { 
        for (size_t i = 0; i < reply->elements; i++) {
            redisReply* message = reply->element[i];
            redisReply* fields = message->element[1];

            // Assuming the message is stored as key-value pairs, and you want the value (not the key)
            for (size_t j = 1; j < fields->elements; j += 2) {
                response += fields->element[j]->str; // Append the value to the response buffer

                if (j + 2 < fields->elements) {
                    response += ", ";  // Add a comma and space between multiple values
                }
            }
        }
    }
    freeReplyObject(reply);
}
void create_consumer_group(redisContext* c, const std::string& stream_name, const std::string& group_name) {
    redisReply* reply = (redisReply*)redisCommand(c, "XGROUP CREATE %s %s $ MKSTREAM", stream_name.c_str(), group_name.c_str());

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        printf("ERROR: %s\n", reply->str);
    } else {
        printf("Consumer group %s created on stream %s\n", group_name.c_str(), stream_name.c_str());
    }

    freeReplyObject(reply);
}

int acknowledge_message(redisContext *c, const std::string& stream_name, const std::string& group_name, const std::string& message_id) {
    std::string command = "XACK " + stream_name + " " + group_name + " " + message_id;
    redisReply *reply = (redisReply *)redisCommand(c, command.c_str());

    if (reply == NULL) {
        std::cerr << "Command execution error\n";
        return false;
    }

    int acknowledged = 0;

    if (reply->type == REDIS_REPLY_INTEGER) {
        if (reply->integer > 0) {
            std::cout << "Acknowledged " << reply->integer << " messages\n";
            acknowledged = 1;
        } else {
            std::cout << "Failed to acknowledge message\n";
        }
    } else {
        std::cout << "Failed to acknowledge message, unexpected reply type: " << reply->type << std::endl;
    }

    freeReplyObject(reply);
    return acknowledged;
}
void check_pending_messages(redisContext* c, const std::string& stream_name, const std::string& group_name) {
    redisReply* reply = (redisReply*)redisCommand(c, "XPENDING %s %s", stream_name.c_str(), group_name.c_str());

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 4) {
        printf("Pending count: %lld\n", reply->element[0]->integer);
        printf("Smallest Pending ID: %s\n", reply->element[1]->str);
        printf("Largest Pending ID: %s\n", reply->element[2]->str);
        printf("Consumers with pending messages: %d\n", (int)reply->element[3]->elements);
    } else {
        printf("No pending messages.\n");
    }

    freeReplyObject(reply);
}
void delete_stream(redisContext* c, const std::string& stream_name) {
    redisReply* reply = (redisReply*)redisCommand(c, "DEL %s", stream_name.c_str());

    if (reply == NULL) {
        printf("Command execution error\n");
    } else if (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
        printf("Stream '%s' deleted successfully.\n", stream_name.c_str());
    } else {
        printf("Stream '%s' does not exist or could not be deleted.\n", stream_name.c_str());
    }

    freeReplyObject(reply);
}
void enqueue_task(redisContext* c, const std::string& queue_name, const std::string& task) {
    redisReply* reply = (redisReply*)redisCommand(c, "RPUSH %s %s", queue_name.c_str(), task.c_str());
    if (reply->type == REDIS_REPLY_INTEGER) {
        printf("Task added to queue '%s'. Queue length: %lld\n", queue_name.c_str(), reply->integer);
    }
    freeReplyObject(reply);
}

void dequeue_task(redisContext* c, const std::string& queue_name, std::string& taskBuffer) {
    redisReply* reply = (redisReply*)redisCommand(c, "LPOP %s", queue_name.c_str());
    if (reply->type == REDIS_REPLY_STRING) {
        printf("Dequeued task: %s\n", reply->str);
        taskBuffer = reply->str;
    } else {
        printf("Queue is empty or command failed.\n");
    }
    freeReplyObject(reply);
}
void print_queue(redisContext* c, const std::string& queue_name) {
    // Fetch all elements from the queue using LRANGE 0 -1
    redisReply* reply = (redisReply*)redisCommand(c, "LRANGE %s 0 -1", queue_name.c_str());

    if (reply == NULL) {
        printf("Command execution error\n");
        return;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        printf("Queue '%s' contains %zu elements:\n", queue_name.c_str(), reply->elements);
        for (size_t i = 0; i < reply->elements; i++) {
            printf("%zu: %s\n", i + 1, reply->element[i]->str);
        }
    } else {
        printf("Queue '%s' is empty or command failed.\n", queue_name.c_str());
    }

    freeReplyObject(reply);
}

int queue_len(redisContext* c, const std::string& queue_name) {
    // Execute the LLEN command to get the length of the list
    redisReply* reply = (redisReply*)redisCommand(c, "LLEN %s", queue_name.c_str());

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
