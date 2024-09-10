#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sx1262.h"
#include "helpFunctions.h"
#include "redis.h"

const char *queue_name = "myqueue";
const char *queue_name_2 = "rxqueue";
// Function prototypes
void *redis_thread_func(void *ptr);
void *lora_thread_func(void *ptr);
int chip_handle;
int spi_handle;
int tx_mode_flag = 0;
int main() {
    puts("hello world");

	redisContext * c = redis_init("127.0.0.1", 6379);
	// Clear the queue at startup
    redisCommand(c, "DEL %s", queue_name);
    redisCommand(c, "DEL %s", queue_name_2);
	//spi lora stuff
	chip_handle = lgpio_init();
	spi_handle = spiHandle(0, 0, 5000000, 0);
	gpio_init(chip_handle);
	factoryReset();


    // Create Redis and LoRa threads
    pthread_t redis_thread, lora_thread;

    int retVal = pthread_create(&redis_thread, NULL, redis_thread_func, NULL);
    int retVal1 = pthread_create(&lora_thread, NULL, lora_thread_func, NULL);

    if (retVal) {
        fprintf(stderr, "Error creating Redis thread: %d\n", retVal);
        return 1;
    }

    if (retVal1) {
        fprintf(stderr, "Error creating LoRa thread: %d\n", retVal1);
        return 1;
    }

    pthread_join(redis_thread, NULL);
    pthread_join(lora_thread, NULL);

    return 0;
}

void *lora_thread_func(void *ptr) {
    redisContext *c = redis_init("127.0.0.1", 6379); //not thread safe

    while (1) {  // Assuming you want to continuously check the queue
        rx_mode_attempt();
		printf("SENDING MESSAGE");
		int len = queue_len(c, queue_name);
        while (len > 0) {
            char taskBuffer[255];
			memset(taskBuffer, 0, 255);
			dequeue_task(c, queue_name, taskBuffer);
				uint16_t payload_len = strlen(taskBuffer)+ 1;
				send_packet((uint8_t* )taskBuffer, payload_len);
				printf("packet has been sent\n");
				len = queue_len(c, queue_name);
				sleep(4);
        }
		tx_mode_flag = 0;
	//adding tasks to send to the CDP people 
	sleep(5);
    }
    redisFree(c);  // Clean up Redis context
    return NULL;
}


void *redis_thread_func(void *ptr) {
	redisContext *c = redis_init("127.0.0.1", 6379);

	const char *stream_name = "mystream";
	const char *group_name = "loraGroup";
	const char *consumer_name = "consumer";
	char keyBuffer[100];
	char messageBuffer[255];
	char messageID[100];

	char keyBuffer_2[100];
	char messageBuffer_2[100];
	char messageID_2[100];
	create_consumer_group(c, stream_name, group_name);

	size_t maxMessages = 100;

	//create messageArray
	KeyValuePair * messageArray = malloc(maxMessages * sizeof(KeyValuePair));
	if (messageArray == NULL) {
		fprintf(stderr, "Mem allocation failed\n");
		return (void *) 1;
	}

    
	while (1) {  // Assuming you want to continuously read from the stream
		size_t totalMessages = 0;
		read_from_consumer_group_dynamic(c, stream_name, group_name, consumer_name, &messageArray, &totalMessages);

		for (int i = 0; i < totalMessages; i++){
			if (strcmp(messageArray[i].key, "CDP_LORA") == 0) {
				printf("Key received: %s\n", messageArray[i].key);
				printf("Message Received: %s\n", messageArray[i].value);
			
			enqueue_task(c, queue_name, messageArray[i].value);
			acknowledge_message(c, stream_name,group_name, messageArray[i].messageID);
			check_pending_messages(c, stream_name, group_name, messageID_2);
			print_queue(c, queue_name);
			tx_mode_flag = 1;

			}
			else if(((strcmp(messageArray[i].key,   "WEB_CDP") == 0) | 
				(strcmp(messageArray[i].key, "CDP_WEB") == 0)) | 
				(strcmp(messageArray[i].key, "LORA_CDP")==0)) {
				printf("ack message: %s\n", messageArray[i].messageID);
				acknowledge_message(c, stream_name, group_name, messageArray[i].messageID);	
			}


			if (messageArray[i].key == NULL) {
			printf("No message received key buffer is empty\n");
		}
		}

		int rx_queue_len = queue_len(c, queue_name_2);
		if (rx_queue_len > 0) {
			puts("queue has contents");
			print_queue(c, queue_name_2);
			const char * key = "LORA_CDP"; //lora --> cdp
			const char * mystream = "mystream";
			char value[255];
			char response[10];
			dequeue_task(c, queue_name_2, value);
			if (strlen(value) > 27) { 
			publish(c, mystream, key, value, response);
			printf("\n\n\n message has been published : %s\n\n\n", value);
			}
			else {
				printf("task did not have a len greater than 27, minimum for header\n");
			}
		}
		else {
			puts("nothing in rx queue");
			}
		sleep(10);
	}
	redisFree(c);  // Clean up Redis context
	return NULL;
}


