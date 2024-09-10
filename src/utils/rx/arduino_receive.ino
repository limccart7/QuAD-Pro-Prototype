#include <LoRa.h>

#define CS_PIN 18
#define RST_PIN 23
#define IRQ_PIN 26

#define BANDWIDTH 250E3
#define SPREADING_FACTOR 12
#define CODING_RATE 5
#define GAIN_LEVEL 6
#define FREQ 915E6
#define OCP_MA 240

volatile bool packetReceived = false;
char receivedData[256]; // Adjust size according to the expected maximum data length
int packetSize = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(CS_PIN, RST_PIN, IRQ_PIN);
  if (!LoRa.begin(FREQ)) {
    Serial.println("LoRa init failed. Check your connections");
    while (true);
  }

  LoRa.disableCrc();
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setCodingRate4(CODING_RATE);
  LoRa.setOCP(OCP_MA);
  LoRa.setGain(GAIN_LEVEL);

  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop() {
  if (packetReceived) {
    packetReceived = false; // Clear the flag

    Serial.print("RSSI: ");
    Serial.print(LoRa.packetRssi());
    Serial.print(" SNR: ");
    Serial.print(LoRa.packetSnr());
    Serial.print(" Length: ");
    Serial.print(packetSize);
    Serial.print(" Data: ");
    
    // Print the received string
    Serial.println(receivedData);
    
    // Print each byte as a hexadecimal value for debugging
    Serial.print("Hex Data: ");
    for (int i = 0; i < packetSize; i++) {
      Serial.print(receivedData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void onReceive(int size) {
  packetSize = size;
  int i;
  for (i = 0; i < size && i < sizeof(receivedData) - 1; i++) {
    receivedData[i] = (char)LoRa.read();
  }
  receivedData[i] = '\0'; // Null-terminate the string
  packetReceived = true; // Set flag to indicate packet received
}

