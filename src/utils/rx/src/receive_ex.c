#include <stdio.h>
#include <string.h>
#include "sx1262.h"
#include "sx1262x_defs_custom.h"
#include "helpFunctions.h"

int chip_handle;
int spi_handle;

int main(){

	printf("start of rx example\n");
	chip_handle = lgpio_init();
	gpio_init(chip_handle);

	spi_handle = spiHandle(0,0,5000000,0);

	if (spi_handle <0 ) {
	printf("SPI port failed to open: error code %d\n", spi_handle);

	return -1;
	}

	factoryReset();
	wait_on_busy();

	rx_mode_attempt();

}
	

