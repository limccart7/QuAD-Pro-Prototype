#include <stdio.h>
#include <stdint.h>
#include <math.h>
#ifndef spi_loopback_h
#define spi_loopback_h

#define BUSY_PIN 16 //J1 Header, pin 4
#define SX_NRESET_PIN 26 //J4 Header, pin 1
#define DIO_PIN 6 //using for tx and rx done 
				  //J1 pin 6
#define CS_PIN 5 //J1 pin 8
#define ANT_SW 25 //J2 pin 1


#define LOW 0
#define HIGH 1

#define READY 0
#define BUSY 1
#define CLEAR_ALL_IRQ 0x03FF
//MARCO for delay_ms 
#define SLEEP_MS(ms) lguSleep((ms) / 1000.0)
//function protypes
int lgpio_init(void);
int spiHandle(int spiDev, int spiChannel, int spiBaud, int spiFlag);
void printBuffer(const char *buffer, int len);
int getStatus(int spi_handle, char opcode, char *response, int response_len);
void gpio_init(int chip_handle);
void wait_on_busy(void);
void tx_mode_attempt(uint8_t* data, uint16_t len);
void receive();
void sendCommand(int spi_handle, uint8_t command, uint8_t* data, uint8_t command_len);
int send_read_command(int spi_handle, uint8_t command, uint8_t* data, int command_len);
//set function protypes

void set_standby_mode();
void set_packet_type(uint8_t packet_type);
void set_rf_frequency(uint32_t frequency_mhz);
void set_pa_config(uint8_t pa_duty_cycle, uint8_t hp_max, uint8_t device_sel);
void set_tx_params(int8_t power, uint8_t ramp_time);

uint8_t read_registers(uint16_t reg_addr, uint8_t* data, uint8_t len);

void set_buffer_base_addr(uint8_t tx_base_addr, uint8_t rx_base_addr);

uint8_t write_registers(uint16_t reg_addr, uint8_t* data, uint8_t len);

void factoryReset();

uint8_t write_buffer(uint8_t offset, uint8_t* data, uint16_t len);

void config_modulation_params(uint8_t spreading_factor, uint8_t bandwidth, uint8_t coding_rate, uint8_t low_data_rate_opt);
void config_packet_params(uint8_t preamble_length, uint8_t header_type, uint8_t payload_length, uint8_t crc_type, uint8_t invert_iq);
void set_dio_irq_params(uint16_t irq_mask, uint16_t dio1_mask, uint16_t dio2_mask,
						uint16_t dio3_mask);

void tx_mode_attempt(uint8_t* data, uint16_t);

void clear_tx_irq();
void wait_on_DIO_IRQ();
uint16_t get_irq_status();

void clear_irq_status(uint16_t status_mask);

void set_regulator_mode();
uint8_t get_status();
uint16_t get_device_errors();
void print_status_information();
void print_device_errors();
void clear_device_errors();
void set_tx_mode(uint32_t timeout);
void wait_on_TX_IRQ(void);
void tx_mode(void);
void nss_select(void);
void nss_deselect(void);
void set_rx_mode(uint32_t);
void rx_mode_attempt();
void ant_sw_on();
void ant_sw_off();
void wait_on_RX_IRQ();
void get_rx_buffer_status(uint8_t* payload_len, uint8_t* rx_start_buff_addr);
uint8_t read_buffer(uint8_t offset, uint8_t*data, uint16_t len);


#endif


