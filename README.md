# Project OWL QUAD PRO

Hello,

The work in this project is for Project-OWL Summer Research with a goal to replicate a duck radio from OWL Integrations. The project has three parts: Web Interface, Cluster Duck Protocol control, and LoRa. The web interface program is responsible for creating a wifi hotspot and processing user input. The Cluster Duck Protocol program takes user input and forms a CDP packet. And finally the LoRa program controls the sx1262 chip to send out the CDP Packet and listening for incoming messages. 

## Redis Stream

The three portions of the project (Web, CDP, LoRa) are all individual programs and must all run at the same time. To share information between the three scripts Redis Streams is used. Redis is an open-source, in-memory data structure store that can be used as a database, cache, and message broker.

![Redis-Stream-Arch](https://github.com/user-attachments/assets/f8cb6003-cf7f-421f-adc2-c0d5ab0054d0)
### Figure 1: Redis Stream Architecture

### Redis-Explained
Redis Streams is primarily used a message broker in this application, but also serves as backup if the redis-server were to fail so no data would be lost. Redis Streams were selected because the stream can be used in a non-blocking fashion. Unlike Redis Pub/Sub, there is no real time requirement to exchange information between programs. Each program acts a consumer group. Using consumer groups allows each program (consumer group) to individually process the data being publised to the stream. Every messages can be seen by all consumer groups but each consumer group will process the information differently. 


VERY IMPORTANT: The consumer group is not able to see messages that exist in the stream before the consumer group has been created. Therefore, in the start script each consumer group should be created before the first message is sent. 


### Publishing
Publishing to a redis stream works with key value pair. The key is used to denote the sender and receiver of the message. The current implemention with just the three consumer groups for the key is: sender_receiver. For example, if the CDP team is sending a packet to LoRa: CDP_LORA.



As a message broker, user input from the web team is formatted into a string which is then published to the redis stream. 

The architecture for the redis stream is simple but dynamic enough to allow for a physical layer to be added. Physical layer refers to how the message will be sent out, the current implementation is LoRa, but other technologies, such as, FSK can be implemented in a similar fashion. 

### Web App
The “web interface” is simply a portal hosted on a client device that allows a user to send messages using a QUAD Pro duck. The functionality on desktop and mobile is exactly the same, which is key to the goals of the project. It should be as accessible as possible for any end-user regardless of their device or location. The bottom of the page is a chat box that functions similarly to a text message interface. Sent messages are displayed justified on the right, and received are on the left.

### CDP 
The CDP (Cluster-Duck-Protocol) directory has code that takes messages from the web-interface to tell the radio to behave as a Ducklink, PapaDuck, MamaDuck, or a DetectorDuck as well as the data to put into the CDP packet if the radio is going to send data. If a CDP packet needs to be formed it will send the packet to the LoRa section to transmit. If the radio is set to handle any incoming received packets it will (a PapaDuck or Mamaduck wait to receive CDP packets) check the packet to see of that particular duck needs to send commands or acknowledgements and send the data back to the LoRa radio if necessary. More information about the ducks in detail and how they operate is in this link: https://docs.google.com/document/d/1uDrMVyc-jzSR5LkHCX8EE5CS4pl8BdmXs_a-XMbBqtw/edit


### LoRa
The LoRa directory in the source folder contains all the code corresponding with the sx1262 physical layer. The code leverages the lg library which enables control of the GPIO pin on the raspberry pi. The sx1262 chip is controlled through SPI. 

On a high level the LoRa portion listens to LoRa messages and redis messages. It forwards the contains of LoRa packet to the CDP team, and transmits the CDP packets from the LoRa side. It is its own consumer group.

### How to run the program



## Cloning the Repo and running install script
```
git clone https://github.com/wyattcolburn/Project-OWL.git
chmod +X install.sh
bash install.sh
```
An installation script has been written to create all the cmake files and download the dependecies. It is recommended to familar yourself with the install script to understand what is being downloaded on your machine. The executables of the CDP and LoRa portions of code will be in their corresponding build files

chmod -X allows the script to be ran. Raspberry pi os uses bash

## CMAKE

This project is built with cmake, within the all the src directories but the Web-Interface (written in Python) have a CMakeLists.txt. The installation script will create all the executables, however, when editting a source file you will have to recompile. Naviagte to the build directory within that source directory and have cmake build the project and the make to create the executables.


```
cd ..
cd build
cmake ..
make
ls 
```
![image](https://github.com/user-attachments/assets/06ab1700-ec3a-4f14-afcd-1174c26f4e52)


## Utility Directory
This directory provides scripts written for quick proof of concepts. The first one you should urn is the spi loopback test to verify that your raspberry pi is working.

### Running Spi Loopback Test
 The first program you should run is spi_loopback_test
 Make sure to short MOSI to MISO (GPIO PINS 10 --> GPIO 9)
 ```
 cd /QuAD-Pro-Prototype/src/utils/

./spi_loopback_test

```
If working correctly, the received datashould be: 0x11, 0x12, 0x13
If it is not working here are some reasons potentially:

1) You have enabled spi within the raspberry pi bios
       a) go to terminal and enter "sudo raspi-config"
       b) naviagate to "Interface Options"
       c) Select SPI, and enable the spi interface.
       d) reboot your raspberry pi
2) In your terminal type "ls /dev/spidev*", this will display all the spi dev ports that one can access. This project uses spidev0.0 which has been tested on the pi4 and pi5. 
3) There were some issues with the pi5, when using the sd card (which had raspberry pi os already installed). A fresh install of raspbian os cleared this issue.
4) Make sure dtparam=spi=on in /boot/firmware/config.txt (this needs to be opened with sudo privilleges to edit)

#### Receive Proof of Concept
Located in /QuAD-Pro-Prototype/src/utils/rx
You can run the example code, it will only receive one message
```
./receive_ex
```

#### Transmit Proof of Concept
Located in /QuAD-Pro-Prototype/src/utils/tx
```
./transmit_ex "your message"
```

### Redis Examples
Atleast two terminals are needed, three is often used as it is easier to tell what is going on. 
To run redis, a server process needs to be created Open a terminal and type 

```
redis-server
```
![image](https://github.com/user-attachments/assets/7333c1d8-e8d6-4705-bfac-a7b6a977121b)
If you get this, it means a redis-server is already running and wasn't destroyed correctly: 

```
sudo systemctl stop redis-server
```
![image](https://github.com/user-attachments/assets/bb894aa9-6893-4bed-94b6-7a3cd74dc797)
This is an indicator of a successful operation

After creating a server, a consumer group is needed and a publisher is needed. In the QuAD-Pro-Prototype/src/utils/messageBrokerthere are the following executables:

"delete_stream" --this executable clears the stream, streams will store old messages so when you purely read the stream you will see every message published to the stream for as long as it has been online

"publish_custom" --this executable publishes a custom message with a predefined key, you have to edit source file and recompile to change the key

"redis_stream" --reads the stream, does not use consumer group, it will show every message in the stream

"redis_consumer" --creates a consumer group, if consumer group already exists throws an error but it does not matter, will then read that stream from that. This program acknowledges messages so you will not                    see the same message if the executable is run multiple times

### This example is to show some of the principles of redis streams
In the first terminal we will start the server
```
sudo systemctl stop redis-server
redis-server

```
In the second terminal we will delete the stream to ensure we are working with a new stream

```
./delete_stream

```
Then we will read the stream, nothing should be in the stream
```
./redis_stream
```
We are going to create the consumer group before publishing messages to avoid missing the message
```
./redis_consumer
```

Now in lets publish a message
```
./publish_custom "hello world"
```
Now lets read the stream to see the message
```
./redis_stream
```
What about the reading it as a consumer
```
./redis_consumer
```
lets publish another message
```
./publish_custom "second message"
```
Lets read the stream again, notice we see both messages
```
./redis_stream
```
When we read as a consumer we only see the second message because we already acknowledged the first message
```
./redis_consumer
```

![image](https://github.com/user-attachments/assets/d924ec82-f5ad-46dc-bd8c-69cb85d432d7)
![image](https://github.com/user-attachments/assets/e223bc29-06d8-47f1-81a6-73611ccafaa1)
![image](https://github.com/user-attachments/assets/fabd85ef-86e3-4865-b0ab-10c05628df18)



If any issues arise please create an issue on the github repo and email me at 
wdcolbur@calpoly.edu

