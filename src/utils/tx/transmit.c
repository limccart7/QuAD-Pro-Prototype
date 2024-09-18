#include <stdio.h>
#include <lgpio.h>
//#include "transmit.h"
#include "sx1262x_defs_custom.h"
#include "helpFunctions.h"
#include "string.h"
#include "sx1262.h"
#define GET_STATUS_OP                   UINT8_C(0xC0)
#define GET_STATS_OP                    UINT8_C(0x10)
#define GET_RST_STATS_OP                UINT8_C(0x00)
/*Pin Connections:*/

/*LoRa Shield -> Raspberry Pi*/
/*J1: */
	/*BUSY 4*/
	/*DIO1 6*/
	/*NSS 8*/

/*J2:*
	/*ANT SW 1*/
	/*D9 2 */
	/*MOSI 4->19 (white cable)*/
	/*MISO 5 ->21 (black cable)*/
	/*SCLK 6 ->23 (grey cable)*/
	/*GND 7*/

/*J3:*/
	/*GND 6 & 7*/
	/*VCC MBED, VDD 3V3, VDD_RADIO 4*/

/*J4:	*/
	/*SX RESET 1*/
	/*VDD 3V3 /GND 2*/
	/*GND 3*/
	/*VCC (XTAL) 4 -> Pin 1*/
	/*A5 5*/
	/*A6 6*/

int enterMessage(char *messageBuffer);
uint16_t count_characters(const char *input);
// Function prototypes
int chip_handle = 0;
int spi_handle = 0;


int main(int argc, char *argv[]) {
    
	puts("start");
    //gpio init
	chip_handle = lgpio_init();
	gpio_init(chip_handle);
	//put GPIO 26 as output for NSS-Reset(needs to be high)
    // Open the SPI device, spi_handle is a handle
    spi_handle = spiHandle(0, 0, 5000000, 0);
    if (spi_handle < 0) {
        printf("SPI port failed to open: error code %d\n", spi_handle);
        return 1;
    }
	
	factoryReset();
	wait_on_busy(); //so waiting for standby mode
	
	char messageBuff[100];

	//user has provided a message
	if (argc < 2) {
		printf("Usage: %s <message>\n", argv[0]);
		return 1;
	}

	// Copy the message to the buffer
    strncpy(messageBuff, argv[1], sizeof(messageBuff) - 1);
    messageBuff[sizeof(messageBuff) - 1] = '\0'; // Ensure null-termination
		

	//function that will take from buffer -->
	send_packet((uint8_t *)messageBuff, count_characters(messageBuff)+ 1);	

	int dio_status = gpio_status(chip_handle, DIO_PIN);
	(dio_status ==  0) ? puts("tx done") : puts("tx error");

	int closeStatus = lgSpiClose(spi_handle);
    if (closeStatus < 0) {
        printf("Failed to close SPI device: error code %d\n", closeStatus);
        return 1;
    }
    printf("SPI device closed successfully\n");

    return 0;
}

