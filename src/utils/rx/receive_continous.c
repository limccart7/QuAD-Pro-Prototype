#include <stdio.h>
#include <lgpio.h>
#include "helpFunctions.h"
#include "sx1262x_defs_custom.h"
#include "helpFunctions.h"
#include "transmit.h"
#define GET_STATUS_OP                   UINT8_C(0xC0)
#define GET_STATS_OP                    UINT8_C(0x10)
#define GET_RST_STATS_OP                UINT8_C(0x00)
/*Pin Connections:*/

/*LoRa Shield -> Raspberry Pi*/
/*J1: */
	/*BUSY 4*/
	/*DIO1 6*/
	/*NSS 8*/

/*J2:*/
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


// Function prototypes
int chip_handle = 0;
int spi_handle = 0;
int interruptFlag = 0;
int main() {
    
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
	
	get_irq_status();
	/*print_status_information();*/

	/*set_regulator_mode();*/
	/*print_status_information();*/
	/*set_standby_mode(); //into standby rc*/
						


	receive();

	int closeStatus = lgSpiClose(spi_handle);
    if (closeStatus < 0) {
        printf("Failed to close SPI device: error code %d\n", closeStatus);
        return 1;
    }
    printf("SPI device closed successfully\n");

    return 0;
}

void nss_select(){
	/*
	This function is used for manual control of the CS pin (external gpio,
	not the chip one). This function pulls pin low, should be down before 
	spi transactions
	*/
	int nss_low = lgGpioWrite(chip_handle, CS_PIN, LOW);
	int nss_status = lgGpioRead(chip_handle, CS_PIN);
	
	//blocking code to make sure nss goes low
	/*while (nss_status == HIGH) {*/
		/*SLEEP_MS(1);*/
		/*nss_status = lgGpioRead(chip_handle, CS_PIN);*/
	/*}*/
	/*puts("CS LOW");*/

}
void nss_deselect(){
	int nss_high = lgGpioWrite(chip_handle, CS_PIN, HIGH);
	
	int nss_status = lgGpioRead(chip_handle, CS_PIN);
	
	//blocking code to make sure nss goes high
	/*while (nss_status == LOW) {*/
		/*SLEEP_MS(1);*/
		/*nss_status = lgGpioRead(chip_handle, CS_PIN);*/
	/*}*/
	/*puts("CS HIGH");*/
}

void wait_on_busy(void){
	//this function checks the busy pin on the Sx1262, busy pin must be low
	//for the chip to receive commands, delays until pin goes low
	
	int busyStatus= lgGpioRead(chip_handle, BUSY_PIN); 
	while (busyStatus == HIGH) {
		SLEEP_MS(1000);
	}
}


void clear_tx_irq(){

	sendCommand(spi_handle, CLEAR_IRQ_STATUS_OP, 0x0, 1); //clear out the first bit
														  
	
														  
	//not sure how to do this? Think I am clearing all the status
	/*uint8_t NOP_buffer[2] = {NO_OPERATION, NO_OPERATION};*/
	/*uint16_t  tx_irq_status = read_registers(GET_IRQ_STATUS_OP, NOP_buffer, 2);*/
	
	/*tx_irq_status &= ~TX_DONE_MASK;*/

	/*write_registers(CLEAR_IRQ_STATUS_OP, (uint8_t *)tx_irq_status, 1);*/
}

void clear_rx_irq() {
	sendCommand(spi_handle, CLEAR_IRQ_STATUS_OP, (uint8_t*) 1, 1);
}
uint16_t get_irq_status() {

	//this function needs work
	uint8_t irq_buff[4];
	uint16_t result;

	getCommand(spi_handle, GET_IRQ_STATUS_OP, irq_buff, 4);
	  result = ((irq_buff[2] & 0x0003) <<8) & (irq_buff[3] &0x00FF);
	/*result = ((irq_buff[0] & 0x0003) << 8) & (irq_buff[1] & 0x00FF);*/
	//only care about the last 2 bits, then shift them to the right
	//only care about last 8 bits? combine tham
	//why what is going on, this tells status on all irq, there are only 10
	//so you can look at which bits are high for status??

	return result;
}

void clear_irq_status(uint16_t status_mask){

	uint8_t irq_buff[2] = {(uint8_t) (((uint16_t) status_mask >> 8) & 0x0003), (uint8_t) ((uint16_t) status_mask & 0x00FF)};

	sendCommand(spi_handle, CLEAR_IRQ_STATUS_OP, irq_buff, 2);

}


uint8_t write_registers(uint16_t reg_addr, uint8_t * data, uint8_t len) {
	puts("writing to register");

	//ISSUES MIGHT ARISE WITH NSS going high and low through multiple spi operations
	nss_select();

	uint8_t status;
	uint8_t cmd_addr[3] = {WRITE_REG_OP, (uint8_t) ((reg_addr >> 8) & (0x00FF)),
						  (uint8_t) (reg_addr & 0x00FF)};
	uint8_t spiWrite = lgSpiWrite(spi_handle, (const char *) cmd_addr, 3);
	uint8_t spiWrite2 = lgSpiWrite(spi_handle, (const char *) data, len - 1);
	uint8_t spiRead_Write = lgSpiXfer(spi_handle, (const char *) data + (len -1),(char *) &status, 1);

	nss_select();
	wait_on_busy();

	return status;

}
uint8_t read_registers(uint16_t reg_addr, uint8_t* data, uint8_t len){
	puts("reading registers");

	uint8_t status;
	uint8_t cmd_addr[3] = {READ_REG_OP, (uint8_t) ((reg_addr >> 8) & (0x00FF)),
						  (uint8_t) (reg_addr &0x0FF)};

	nss_select(); //should go low
	
	int spiWrite = lgSpiWrite(spi_handle, (const char *) cmd_addr, 3); 
	/*(spiWrite >= 0) ? puts("spi write read") : puts("cannot write");*/

	int spiRead_Write = lgSpiXfer(spi_handle, (const char *) NO_OPERATION, (char *) &status, 1);//dif from quad, 
	int spiRead = lgSpiRead(spi_handle, (char *) data, len); //reads data into paramters
	
	nss_deselect(); //nss goes back high
	wait_on_busy();

	return status;
}

void receive(){
	int len = 100;

	set_standby_mode();
	print_status_information();
	set_packet_type(LORA_PKT_TYPE);
	set_rf_frequency(915000000);
	set_buffer_base_addr(0x00, 0x00);
	config_modulation_params(LORA_SF_12, LORA_BW_500, LORA_CR_4_5, 0) ;
	config_packet_params(12, PKT_EXPLICIT_HDR, len, PKT_CRC_OFF, STD_IQ_SETUP);

	set_dio_irq_params(0xFFFF, RX_DONE_MASK, 0x0000, 0x0000); //sets dio1 as tx
	set_rx_mode(0xFFFFFF); //continous mode
						   //
	wait_on_busy();
	print_status_information();	   
	wait_on_DIO_IRQ();
	print_status_information();	   
	
	
	while (interruptFlag == 0) {
		wait_on_DIO_IRQ();
	//should clear irq
		uint8_t payload_len = 0; 
		uint8_t rx_buff_st_addr = 0;
		uint8_t rx_pkt[256];

		get_rx_buffer_status(&payload_len, &rx_buff_st_addr);
		printf("payload len  ");
		printf("%d\n", payload_len);
		read_buffer(rx_buff_st_addr, rx_pkt, payload_len);

		for(int i = 0; i < payload_len; i++)
		{
			printf("%c", rx_pkt[i]);
		}
		printf("\n");
		clear_irq_status(0x03FF);
		clear_irq_status(0x03FF); //i have no idea why this needs to sent twice for it to work, but twice for rx will reset once for tx????
	}
}

void set_rx_mode(uint32_t timeout) {

	ant_sw_on();

	SLEEP_MS(100);
	uint8_t rx_gain = RX_GAIN_PWR_SAVING;
	write_registers(REG_RX_GAIN, &rx_gain, 1);
	wait_on_busy();
	uint8_t readRegVal;
	read_registers(REG_RX_GAIN, &readRegVal, 1);
	printf("0x%02X\n", readRegVal);
	uint8_t timeout_buff[3];
	timeout_buff[0] = (uint8_t) (timeout >> 16) & 0xFF;
    timeout_buff[1] = (uint8_t) (timeout >> 8) & 0xFF;
    timeout_buff[2] = (uint8_t) timeout & 0xFF;

	sendCommand(spi_handle, SET_RX_MODE_OP, timeout_buff, 3);

}


//tx functions
void set_standby_mode(){
	//0 for RC
	//1 for XOCX
	ant_sw_off();
	sendCommand(spi_handle, SET_STANDBY_OP, 0, 1); //step 1, standby rc
	wait_on_busy();
}
void set_packet_type(uint8_t packet_type){
	if(packet_type == LORA_PKT_TYPE ||
        packet_type == GFSK_PKT_TYPE)
    {
		sendCommand(spi_handle, SET_PACKET_TYPE_OP, &packet_type, 1);
	}
}

void set_rf_frequency(uint32_t frequency_mhz){
	uint8_t buff[4];
    uint32_t sx_freq = 0;

    // Calculate RF frequency
    sx_freq = (uint32_t)((double) frequency_mhz / (double) FREQ_STEP);

    buff[0] = (uint8_t) ((sx_freq >> 24) & 0xFF);
    buff[1] = (uint8_t) ((sx_freq >> 16) & 0xFF);
    buff[2] = (uint8_t) ((sx_freq >> 8) & 0xFF);
    buff[3] = (uint8_t) (sx_freq & 0xFF);

	sendCommand(spi_handle, SET_RF_FREQUENCY_OP, buff, 4);

}

void set_pa_config(uint8_t pa_duty_cycle, uint8_t hp_max, uint8_t device_sel){
//page 77 of data sheet
	if(pa_duty_cycle > 0x04) //this is above the spec, should not happen 
        return; // Do nothing in this case

    if(hp_max > 0x07) // Clamp to the recommended max as per datasheet
        hp_max = 0x07;

    if(device_sel != 0x00 && device_sel != 0x01)
        return; // Do nothing in this case

    uint8_t pa_config_buff[4] = {pa_duty_cycle, hp_max, device_sel, 0x01};
	sendCommand(spi_handle, SET_PA_CONFIG_OP, pa_config_buff, 4);
}

void set_tx_params(int8_t power, uint8_t ramp_time){

	uint8_t tx_clamp_config_val;
	read_registers(REG_TX_CLAMP_CONFIG, &tx_clamp_config_val, 1);
	tx_clamp_config_val |= 0x1E;
    write_registers(REG_TX_CLAMP_CONFIG, &tx_clamp_config_val, 1);


	set_pa_config(0x04, 0x07, SX1262_DEV_TYPE);
	if (power < -9 ){
		power = -9;
	}
	else if (power > 22) {
		power = 22;
	}
	uint8_t ocp_setting = OCP_SX1262_140_MA;
	write_registers(REG_OCP_CONFIG, &ocp_setting, 1); //whhy &

	/*wait_on_busy();*/
	/*uint8_t reg_reading;*/
	/*read_registers(REG_OCP_CONFIG, &reg_reading, 1);*/
	/*printf("0x%02X\n", reg_reading);*/

	if (ramp_time > SET_RAMP_3400U) {
		ramp_time = SET_RAMP_3400U;
	}
	uint8_t tx_config_buff[2] = {power, ramp_time};
	sendCommand(spi_handle, SET_TX_PARAM_OP, tx_config_buff, 2);
}
void set_buffer_base_addr(uint8_t tx_base_addr, uint8_t rx_base_addr){
	uint8_t addr_buff[2] = {tx_base_addr, rx_base_addr};

	sendCommand(spi_handle, SET_BUFFER_BASE_ADDR_OP, addr_buff, 2);
}
uint8_t write_buffer(uint8_t offset, uint8_t* data, uint16_t len){

	uint8_t status;
	uint8_t cmd_offset[2] = {WRITE_BUF_OP, offset};

	nss_select();
	int spiWrite = lgSpiWrite(spi_handle, (const char *) cmd_offset, 2);
	int spiWrite2 = lgSpiWrite(spi_handle, (const char *) data, len - 1);
	int spiReadWrite = lgSpiXfer(spi_handle, (const char *) data + (len +1 ), (char *) &status, 1);

	nss_deselect();
	wait_on_busy();

	return status;
}
uint8_t read_buffer(uint8_t offset, uint8_t* data, uint16_t len){

	uint8_t status;
	uint8_t cmd_offset[2] = {READ_BUF_OP, offset};

	nss_select();
	int spiWrite = lgSpiWrite(spi_handle, (const char *) cmd_offset, 2);
	int spiReadWrite = lgSpiXfer(spi_handle, (const char *)NO_OPERATION, (char *)&status, 1);
	int read = lgSpiRead(spi_handle, (char *)(data), len);

	nss_select();
	
	wait_on_busy();
	return status;
}

void config_modulation_params(uint8_t spreading_factor, uint8_t bandwidth, uint8_t coding_rate, uint8_t low_data_rate_opt){

	uint8_t mod_params[4] = {spreading_factor, bandwidth, coding_rate, low_data_rate_opt};

	sendCommand(spi_handle, SET_MOD_PARAM_OP, mod_params, 4);

}

void config_packet_params(uint8_t preamble_length, uint8_t header_type, uint8_t payload_length, uint8_t crc_type, uint8_t invert_iq) {

	uint8_t packet_param_buff[6];

	packet_param_buff[0] = (uint8_t)((preamble_length >>8 ) & 0x00FF);
	packet_param_buff[1] = (uint8_t)((preamble_length & 0x00FF));
	packet_param_buff[2] = (header_type == IMPLICIT_HDR) ? IMPLICIT_HDR : EXPLICIT_HDR;
	packet_param_buff[3] = payload_length;
	packet_param_buff[4] = (crc_type == CRC_ON) ? CRC_ON : CRC_OFF;
	packet_param_buff[5] = (invert_iq == INV_IQ_SETUP) ? INV_IQ_SETUP : STD_IQ_SETUP;

	sendCommand(spi_handle, SET_PACKET_PARAMS_OP, packet_param_buff, 6);
}

void set_dio_irq_params(uint16_t irq_mask, uint16_t dio1_mask, uint16_t dio2_mask,
						uint16_t dio3_mask) {

	uint8_t irq_dio_buff[8];

	 // IRQ Mask
    irq_dio_buff[0] = (uint8_t) ((irq_mask >> 8) & 0x00FF);
    irq_dio_buff[1] = (uint8_t) (irq_mask & 0x00FF);

    // DIO1 Mask
    irq_dio_buff[2] = (uint8_t) ((dio1_mask >> 8) & 0x00FF);
    irq_dio_buff[3] = (uint8_t) (dio1_mask & 0x00FF);

    // DIO2 Mask
    irq_dio_buff[4] = (uint8_t) ((dio2_mask >> 8) & 0x00FF);
    irq_dio_buff[5] = (uint8_t) (dio2_mask & 0x00FF);
    
    // DIO3 Mask
    irq_dio_buff[6] = (uint8_t) ((dio3_mask >> 8) & 0xFF);
    irq_dio_buff[7] = (uint8_t) (dio3_mask & 0x00FF);

    sendCommand(spi_handle, SET_DIO_IRQ_PARMS_OP, irq_dio_buff, 8);
}

void set_tx_mode(uint32_t timeout) {
	//setting to zero is one shot mode, will return to STBY_RC mode afterward
	ant_sw_on();
	uint8_t timeout_buff[3];
    timeout_buff[0] = (uint8_t) (timeout >> 16) & 0xFF;
    timeout_buff[1] = (uint8_t) (timeout >> 8) & 0xFF;
    timeout_buff[2] = (uint8_t) timeout & 0xFF;

	sendCommand(spi_handle, SET_TX_MODE_OP, timeout_buff, 3);
	/*print_status_information();*/

	wait_on_busy();
}
void wait_on_DIO_IRQ(void){

	int dioStatus= lgGpioRead(chip_handle, DIO_PIN); 
	while (dioStatus == LOW) {
		puts("still low");
		SLEEP_MS(1000);
		dioStatus= lgGpioRead(chip_handle, DIO_PIN); 
	}
	puts("dio done!");
}

void get_rx_buffer_status(uint8_t* payload_len, uint8_t* rx_start_buff_addr) {

	uint8_t rx_status[2];

	getCommand(spi_handle, GET_RX_BUFF_STATUS_OP, rx_status, 2);

	*payload_len = rx_status[0];
	*rx_start_buff_addr = rx_status[1];
}

void sendCommand(int spi_hanlde, uint8_t opcode, uint8_t* data, uint8_t len){

	nss_select();

	int Status = lgSpiWrite(spi_handle, (const char *) &opcode, 1);

	if (len > 0) {
		int writeData = lgSpiWrite(spi_handle, (const char *) data, len);
	

		nss_deselect();

		SLEEP_MS(1);

		if (opcode != SET_SLEEP_OP){
			wait_on_busy();
		}
	
	}
}

void send_packet(uint8_t* data, uint16_t data_len) {

	set_buffer_base_addr(0x00, 0x00); //does this order matter
	write_buffer(0x00, data, data_len);

	set_dio_irq_params(0xFFFF, TX_DONE_MASK, 0x0000, RX_DONE_MASK); //setup tx done irq
	set_tx_mode(0x00);

	wait_on_DIO_IRQ();

	//need to clear IRQ

 }
	
uint8_t get_status(){

	uint8_t status;

	status = getCommand(spi_handle, GET_STATUS_OP, NULL, 0);

	return status;
}

void print_status_information(){

	uint8_t status = get_status();

	 switch((status & 0x70) >> 4)
    {
        case 0x2:
            printf("\tChip Mode: STDBY RC MODE\n");
            break;
        case 0x3:
            printf("\tChip Mode: STDBY XOSC MODE\n");
            break;
        case 0x4:
            printf("\tChip Mode: FS MODE\n");
            break;
        case 0x5:
            printf("\tChip Mode: RX Mode\n");
            break;
        case 0x6:
            printf("\tChip Mode: TX Mode\n");
            break;
        default:
            printf("\tChip Mode: Unused/RFU \n");
    }

    switch((status & 0x0E) >> 1)
    {
        case 0x2:
            printf("\tCommand Status: Data is available to host\n");
            break;
        case 0x3:
            printf("\tCommand Status: Command timeout\n");
            break;
        case 0x4:
            printf("\tCommand Status: Command processing error\n");
            break;
        case 0x5:
            printf("\tCommand Status: Failure to execute command\n");
            break;
        case 0x6:
            printf("\tCommand Status: Command TX done\n");
            break;
        default:
            printf("\tCommand Status: Unused/RFU\n");
    }
}
void print_device_errors()
{
    uint16_t errors = get_device_errors();

    if((errors & 0x1FF) == 0x0000)
        printf("\tNo device errors\n");

    if(errors & RC64_CALIB_ERR_MASK)
        printf("\t[SX126X Error]: RC64k Calibration Failed\n");

    if(errors & RC13M_CALIB_ERR_MASK)
        printf("\t[SX126X Error]: RC13M Calibration Failed\n");

    if(errors & PLL_CALIB_ERR_MASK)
        printf("\t[SX126X Error]: PLL Calibration Failed\n");

    if(errors & ADC_CALIB_ERR_MASK)
        printf("\t[SX126X Error]: ADC Calibration Failed\n");

    if(errors & IMG_CALIB_ERR_MASK)
        printf("\t[SX126X Error]: IMG Calibration Failed\n");

    if(errors & XOSC_START_ERR_MASK)
        printf("\t[SX126X Error]: XOSC Failed To Start\n");

    if(errors & PLL_LOCK_ERR_MASK)
        printf("\t[SX126X Error]: PLL Failed To Lock\n");

    if(errors & PA_RAMP_ERR_MASK)
        printf("\t[SX126X Error]: PA Raming Failed\n");
}

uint16_t get_device_errors(){

	uint8_t dev_errors[2];
	getCommand(spi_handle, GET_DEV_ERRS_OP, dev_errors, 2);

	uint16_t combined_errors = (uint16_t) ((dev_errors[0] << 8) & 0x0100) | (dev_errors[1] & 0x00FF);
    return combined_errors;
}
void clear_device_errors(){

	uint8_t clear_buff[2] = {0x00};

	sendCommand(spi_handle, CLR_DEV_ERRS_OP, clear_buff, 2);
}
void set_regulator_mode() {

	uint8_t regu_mode = 0; //ldo mode only

	sendCommand(spi_handle, SET_REG_MODE_OP, &regu_mode, 1);

}

void factoryReset(){

	int low = lgGpioWrite(chip_handle, SX_NRESET_PIN, LOW);

	SLEEP_MS(100);

	int high = lgGpioWrite(chip_handle, SX_NRESET_PIN, HIGH);

	SLEEP_MS(100);

	wait_on_busy();

}

void ant_sw_on(){
	int ant_sw = lgGpioWrite(chip_handle, ANT_SW, HIGH);
}

void ant_sw_off() {
	int ant_sw = lgGpioWrite(chip_handle, ANT_SW, LOW);
}


