#include <stdio.h>
#include <lgpio.h>
#include "spi_loopback.h"

// Function prototype
int main() {
    puts("start");
    lgpio_init();

    // Open the SPI device, spi_handle is a handle
    int spi_handle = spiHandle(0, 0, 9600, 0);
    if (spi_handle < 0) {
        printf("SPI port failed to open: error code %d\n", spi_handle);
        return 1;
    }

    // Transmit and receive data using SPI loopback
    const char txBuf[] = {0x11, 0x12, 0x13}; // Example data to send
    char rxBuf[sizeof(txBuf)] = {0};
    int xferCount = lgSpiXfer(spi_handle, txBuf, rxBuf, sizeof(txBuf));
    if (xferCount < 0) {
        printf("Failed to transfer data with SPI device: error code %d\n", xferCount);
        lgSpiClose(spi_handle);
        return 1;
    }
    printf("Transferred %d bytes with SPI device\n", xferCount);

    // Validate received data
    if (xferCount == sizeof(txBuf)) {
        printf("Success: Received expected data\n");
        printBuffer(rxBuf, xferCount);
    } else {
        printf("Error: Transferred count (%d) does not match buffer size (%lu)\n", xferCount, sizeof(txBuf));
        printBuffer(rxBuf, xferCount);
    }

    // Close SPI device
    int closeStatus = lgSpiClose(spi_handle);
    if (closeStatus < 0) {
        printf("Failed to close SPI device: error code %d\n", closeStatus);
        return 1;
    }
    printf("SPI device closed successfully\n");

    return 0;
}

int lgpio_init(void) {
    uint8_t h;
    h = lgGpiochipOpen(0);

    if (h >= 0) {
        puts("GPIO chip opened");
    } else {
        puts("Failed to open GPIO chip");
    }
    return h;
}

int spiHandle(int spiDev, int spiChannel, int spiBaud, int spiFlag) {
    int spiOpenVal = lgSpiOpen(spiDev, spiChannel, spiBaud, spiFlag);
    if (spiOpenVal >= 0) {
        puts("SPI port open success");
    } else {
        puts("SPI port failed to open");
    }
    return spiOpenVal;
}

void printBuffer(const char *buffer, int len) {
    // This function takes a buffer and outputs it byte by byte
    for (int i = 0; i < len; i++) {
        printf("0x%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
}

