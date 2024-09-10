#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H


#include <stdio.h>
#include <lgpio.h>

//function protypes

int lgpio_init(void);
int spiHandle(int spiDev, int spiChannel, int spiBaud, int spiFlag);

void gpio_init(int chip_handle);
void printBuffer(const char *buffer, int len);
uint8_t getCommand(int spi_handle, uint8_t opcode, uint8_t* data, uint8_t len);
int gpio_status(int chip_handle, const int gpio_pin);

#endif 
