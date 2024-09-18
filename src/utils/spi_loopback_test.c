#include <stdio.h>
#include <lgpio.h>

int lgpio_init(void);
int spiHandle(int spiDev, int spiChannel, int spiBaud, int spiFlag);
void printBuffer(const char *buffer, int len);
int is_raspberry_pi_5(void);

// Function prototype
int main() {
    puts("start");
    int chip_hanlde = lgpio_init();

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
	 int chip_handle = is_raspberry_pi_5() ? 4 : 0; //detects which pi we are on, raspberry pi should open gpiochip
    h = lgGpiochipOpen(0);

    if (h >= 0) {
        puts("GPIO chip opened");
    } else {
        puts("Failed to open GPIO chip");
    }
    return chip_handle;
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
void printBuffer(const char *buffer, int len) {
    // This function takes a buffer and outputs it byte by byte
    for (int i = 0; i < len; i++) {
        printf("0x%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
}
