#include <stdio.h>
#include <lgpio.h>
#include "sx1262.h"
#include "sx1262x_defs_custom.h"
#include "helpFunctions.h"
#include "string.h"
int lgpio_init(void) {
    uint8_t h;
    int chip_handle = is_raspberry_pi_5() ? 4: 0;		
	h = lgGpiochipOpen(4); //opening a connection linux kernel spidev #
							
    if (h >= 0) {
        puts("GPIO chip opened");
    } else {
        puts("Failed to open GPIO chip");
    }
	return h;
 }

int spiHandle(int spiDev, int spiChannel, int spiBaud, int spiFlag) {
    int spiOpenVal = lgSpiOpen(spiDev, spiChannel, spiBaud, spiFlag);

	(spiOpenVal >= 0) ? puts("SPI port success") : puts("SPI port fail");
    return spiOpenVal;
}

void gpio_init(int chip_handle){

	int sx_nreset =  lgGpioClaimOutput(chip_handle,0, SX_NRESET_PIN, HIGH); //init high
	(sx_nreset >= 0 ) ? puts("sx_nreset init") : puts("sx_nreset fail");

	int busy = lgGpioClaimInput(chip_handle, LG_SET_PULL_DOWN, BUSY); //GPIO 16 input
	(busy >= 0) ? puts("busy init") : puts("busy fail");

	int txIRQ = lgGpioClaimInput(chip_handle, LG_SET_PULL_DOWN, DIO_PIN); //gpio 6 input
	(txIRQ >= 0) ? puts("tx init") : puts("tx fail");	
	int ant_sw = lgGpioClaimOutput(chip_handle, 0, ANT_SW, LOW);
	(ant_sw >= 0) ? puts("ANT SW init") : puts("ant sw fail");

	int cs_init = lgGpioClaimOutput(chip_handle, 0, CS_PIN, HIGH);
	(cs_init >= 0) ? puts("CS init") : puts("cs fail");
	
}
void printBuffer(const char *buffer, int len) {
    // This function takes a buffer and outputs it byte by byte
    for (int i = 0; i < len; i++) {
        printf("0x%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
}

int gpio_status(int chip_handle, const int gpio_pin){

	int returnVal = lgGpioRead(chip_handle, gpio_pin);
	return returnVal;
}
uint8_t getCommand(int spi_handle, uint8_t opcode, uint8_t* data, uint8_t len) {

	uint8_t status;

	nss_select();

	int WriteOpcode = lgSpiWrite(spi_handle, (const char *) &opcode, 1);
	int WriteNop = lgSpiXfer(spi_handle, NO_OPERATION, (char *) &status, 1);
	int readresponse = lgSpiRead(spi_handle, (char *) data, len);

	nss_deselect();

	wait_on_busy();

	return status;
}

int is_raspberry_pi_5(void) {
    FILE *fp;
    char buffer[256];

    fp = fopen("/proc/device-tree/model", "r");
    if (fp == NULL) {
        perror("Failed to open /proc/device-tree/model");
        return 0; // Default to not Raspberry Pi 5 if we can't read the model
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        fclose(fp);
        if (strstr(buffer, "Raspberry Pi 5") != NULL) {
            return 1; // It is a Raspberry Pi 5
        }
    } else {
        fclose(fp);
    }

    return 0; // Not a Raspberry Pi 5
}
