# Project OWL QUAD PRO

Hello,

The work in this project is for Project-OWL Summer Research with a goal to replicate a duck radio from OWL Integrations. The project has three parts: Web Interface, Cluster Duck Protocol control, and LoRa. The web interface program is responsible for creating a wifi hotspot and processing user input. The Cluster Duck Protocol program takes user input and forms a CDP packet. And finally the LoRa program controls the sx1262 chip to send out the CDP Packet and listening for incoming messages. 

## Redis Stream

The three portions of the project (Web, CDP, LoRa) are all individual programs and must all run at the same time. To share information between the three scripts Redis Streams is used. Redis is an open-source, in-memory data structure store that can be used as a database, cache, and message broker.

![Redis-Stream-Arch](https://github.com/user-attachments/assets/f8cb6003-cf7f-421f-adc2-c0d5ab0054d0)
### Figure 1: Redis Stream Architecture

### Redis-Explained
Redis Streams is primarily used a message broker in this application, but also serves as backup if the redis-server were to fail so no data would be lost. Redis Streams were selected because the stream can be used in a non-blocking fashion. Unlike Redis Pub/Sub, there is no real time requirement to exchange information between programs. Each program acts a consumer group. Using consumer groups allows each program (consumer group) to individually process the data being publised to the stream. Every messages can be seen by all consumer groups but each consumer group will process the information differently. 

### Publishing
Publishing to a redis stream works with key value pair. The key is used to denote the sender and receiver of the message. The current implemention with just the three consumer groups for the key is: sender_receiver. For example, if the CDP team is sending a packet to LoRa: CDP_LORA.



As a message broker, user input from the web team is formatted into a string which is then published to the redis stream. 

The architecture for the redis stream is simple but dynamic enough to allow for a physical layer to be added. Physical layer refers to how the message will be sent out, the current implementation is LoRa, but other technologies, such as, FSK can be implemented in a similar fashion. 

### Web App
The “web interface” is simply a portal hosted on a client device that allows a user to send messages using a QUAD Pro duck. The functionality on desktop and mobile is exactly the same, which is key to the goals of the project. It should be as accessible as possible for any end-user regardless of their device or location. The bottom of the page is a chat box that functions similarly to a text message interface. Sent messages are displayed justified on the right, and received are on the left.

### CDP 
Description of what CDP does


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


### Running Spi Loopback Test
 The first program you should run is spi_loopback_test
 Make sure to short MOSI to MISO (GPIO PINS 10 --> GPIO 9)
 ```
 cd /QuAD-Pro-Prototype/src/utils/

./spi_loopback_test

```
### File Transfer Test
To simulate the message broker, first the receive and transmit functionality will be tested using files to read messages and write messages. 
This is done in main. Two sx1262 are required, a wiring diagram is linked below, one will run file_test_rx.c and one will run file_test_tx.c. 
The two .txt files simulate the webserver, with input.txt being the user input (transmitted message) and output.txt being what is sent to the user interface (received message). 

```
cd Project-OWL
cd src
cd main
```
### First Run file_test_rx.c in order to not have to worry about missing the transmission.The program will receive a LoRa packet, decode the CDP packet, and write to output.txt
```
./writeFile
```
### The run file_test_tx.c to transmit. The program will read the contents of input.txt, generate a CDP packet and then transmit. 
```
./readFile
```
## Editing files
If you wish to transmit a different message: change the input.txt file. 
If you wish to change the wiring, edit sx1262.h
If you wish to change LORA parameters, edit sx1262.h

After making changes compile the code with the following commands: the first for tx, and the second for rx portion.
```
gcc file_test_tx.c helpFunctions.c cdp.c sx1262.c -o readFile -llgpio
```
```
gcc file_test_rx.c helpFunctions.c cdp.c sx1262.c -o writeFile -llgpio
```

### Running Example Code 
#### RX
Navigate to the rx directory
For one-shot mode, in which you would receive only one message
```
cd files
cd rx
./receive
```
To test continuous mode where the program will run until cancelled and receive messages
```
cd files
cd rx
./receiveCont
```
#### TX
Navigate to tx directory
```
cd files
cd tx
./transmit "your message"
```

## Install GPIO DRIVER
To install LG independent to this project:

``````wget http://abyz.me.uk/lg/lg.zip  
unzip lg.zip  
cd lg  
make  
sudo make install
``````
If you are cloning this repo lg is already installed:
:
cd lg
make 
sudo make install

Source Files are found in /Project-OWL/lg/src

There is a spi_loopback test to ensure spi operations are working, the received data
should be 0x11, 0x12, 0x13

There are rx and tx directories with basic examples. cd into the respective folder and run the file. ./transmit and ./receive 

If you wish to send different messages just edit the transmit.c, line 66 and 68 are the only lines required to edit

After editing a file you must recompile:

Use this command to compile:

gcc filename.c helpFunctions.c -o exeFilename -llgpio

Run the program with:

./exeFilename

If any issues arise please create an issue on the github repo and email me at 
wdcolbur@calpoly.edu

