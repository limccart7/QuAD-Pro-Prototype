#ifndef SX126X_DEFS_H
#define SX126X_DEFS_H

#include <stddef.h>
#include <stdint.h>

// Setup byte packing for structures to treat them as char buffers
//#pragma pack(1) //what is this?

/* General OP-Codes */
#define NO_OPERATION                    UINT8_C(0x00) // NOP

/* Main Functional Modes */
#define SET_SLEEP_OP                    UINT8_C(0x84) // Sleep mode
#define SET_STANDBY_OP                  UINT8_C(0x80) // Standby mode
#define SET_FS_MODE_OP                  UINT8_C(0xC1) // Freq. Synth mode
#define SET_TX_MODE_OP                  UINT8_C(0x83) // Transmit mode
#define SET_RX_MODE_OP                  UINT8_C(0x82) // Recieve mode

#define SET_TMR_ON_PREAM_OP             UINT8_C(0x9F)
#define SET_RX_DUTY_CYCLE_OP            UINT8_C(0x94)
#define SET_RX_CAD_MODE_OP              UINT8_C(0xC5)
#define SET_TX_CONT_WAVE_OP             UINT8_C(0xD1)
#define SET_TX_INFI_PREAM_OP            UINT8_C(0xD2)
#define SET_REG_MODE_OP                 UINT8_C(0x96)
#define SET_CALIBRATE_OP                UINT8_C(0x89)
#define SET_CALIBRATE_IMG_OP            UINT8_C(0x98)
#define SET_PA_CONFIG_OP                UINT8_C(0x95)
#define SET_RX_TX_FALLBACK_MODE_OP      UINT8_C(0x93)

/* DIO/IRQ Control */
#define SET_DIO_IRQ_PARMS_OP            UINT8_C(0x08)
#define GET_IRQ_STATUS_OP               UINT8_C(0x12)
#define CLEAR_IRQ_STATUS_OP             UINT8_C(0x02)
#define SET_DIO2_AS_RF_SW_CTRL_OP       UINT8_C(0x9D)
#define SET_DIO3_AS_TCXO_CTRL_OP        UINT8_C(0x97)

/* RF Mod. and Packet Control */
#define SET_RF_FREQUENCY_OP             UINT8_C(0x86)
#define SET_PACKET_TYPE_OP              UINT8_C(0x8A)
#define GET_PACKET_TYPE_OP              UINT8_C(0x11)
#define SET_TX_PARAM_OP                 UINT8_C(0x8E)
#define SET_MOD_PARAM_OP                UINT8_C(0x8B)
#define SET_PACKET_PARAMS_OP            UINT8_C(0x8C)
#define SET_CAD_PARAMS_OP               UINT8_C(0x88)
#define SET_BUFFER_BASE_ADDR_OP         UINT8_C(0x8F)
#define SET_LORA_SYM_NUM_TIMEOUT_OP     UINT8_C(0xA0)

/* Status Commands */
#define GET_STATUS_OP                   UINT8_C(0xC0)
#define GET_RSSI_INST_OP                UINT8_C(0x15)
#define GET_RX_BUFF_STATUS_OP           UINT8_C(0x13)
#define GET_PACKET_STATUS_OP            UINT8_C(0x14)
#define GET_DEV_ERRS_OP                 UINT8_C(0x17)
#define CLR_DEV_ERRS_OP                 UINT8_C(0x07)
#define GET_STATS_OP                    UINT8_C(0x10)
#define SET_RST_STATS_OP                UINT8_C(0x00)

/* Register Commands */
#define READ_REG_OP                     UINT8_C(0x1D)
#define WRITE_REG_OP                    UINT8_C(0x0D)

/* Buffer Commands */
#define READ_BUF_OP                     UINT8_C(0x1E)
#define WRITE_BUF_OP                    UINT8_C(0x0E)

/* Register Table */
#define REG_DIOX_OUTPUT_EN              UINT16_C(0x0580)
#define REG_DIOX_INPUT_EN               UINT16_C(0x0583)
#define REG_DIOX_PULLUP_CTRL            UINT16_C(0x0584)
#define REG_DIOX_PULLDOWN_CTRL          UINT16_C(0x0585)

#define REG_WHITENING_INIT_MSB          UINT16_C(0x06B8)
#define REG_WHITENING_INIT_LSB          UINT16_C(0x06B9)

#define REG_CRC_INIT_MSB                UINT16_C(0x06BC)
#define REG_CRC_INIT_LSB                UINT16_C(0x06BD)
#define REG_CRC_POLY_INIT_MSB           UINT16_C(0x06BE)
#define REG_CRC_POLY_INIT_LSB           UINT16_C(0x06BF)

#define REG_FSK_SYNC_WORD_0             UINT16_C(0x06C0)
#define REG_FSK_SYNC_WORD_1             UINT16_C(0x06C1)
#define REG_FSK_SYNC_WORD_2             UINT16_C(0x06C2)
#define REG_FSK_SYNC_WORD_3             UINT16_C(0x06C3)
#define REG_FSK_SYNC_WORD_4             UINT16_C(0x06C4)
#define REG_FSK_SYNC_WORD_5             UINT16_C(0x06C5)
#define REG_FSK_SYNC_WORD_6             UINT16_C(0x06C6)
#define REG_FSK_SYNC_WORD_7             UINT16_C(0x06C7)

#define REG_FSK_NODE_ADDR               UINT16_C(0x06C6)
#define REG_FSK_BROADCAST_ADDR          UINT16_C(0x06CE)

#define REG_IQ_POL_SETUP                UINT16_C(0x0736)

#define REG_LORA_SYNC_WORD_MSB          UINT16_C(0x0740)
#define REG_LORA_SYNC_WORD_LSB          UINT16_C(0x0741)

#define REG_RNG_0                       UINT16_C(0x0819)
#define REG_RNG_1                       UINT16_C(0x081A)
#define REG_RNG_2                       UINT16_C(0x081B)
#define REG_RNG_3                       UINT16_C(0x081C)

#define REG_TX_MOD                      UINT16_C(0x0889)
#define REG_RX_GAIN                     UINT16_C(0x08AC)

#define REG_TX_CLAMP_CONFIG             UINT16_C(0x08D8)

#define REG_OCP_CONFIG                  UINT16_C(0x08E7)

#define REG_RTC_CTRL                    UINT16_C(0x0902)
#define REG_XTA_TRIM                    UINT16_C(0x0911)
#define REG_XTB_TRIM                    UINT16_C(0x0912)
#define REG_DIO3_OUTPUT_VOLTAGE         UINT16_C(0x0920)
#define REG_EVENT_MASK                  UINT16_C(0x0944)

/**
 * Useful constants for interaction
*/
#define MODE_SLEEP                      UINT8_C(0x00) // Sleep Mode
#define MODE_STDBY_RC                   UINT8_C(0x01) // Standby RC Mode
#define MODE_STDBY_XOSC                 UINT8_C(0x02) // Standby XOSC Mode
#define MODE_FS                         UINT8_C(0x03) // Freq. Synth. Mode
#define MODE_TX                         UINT8_C(0x04) // Transmit Mode
#define MODE_RX                         UINT8_C(0x05) // Receive Mode
#define MODE_RX_DC                      UINT8_C(0x06) // Receive Duty Cycle Mode
#define MODE_CAD                        UINT8_C(0x07) // Channel Activity Detection Mode

#define FULL_CALIBRATION                UINT8_C(0x7F)

#define DC_DC_LDO_MODE                  UINT8_C(0x01)
#define LDO_ONLY_MODE                   UINT8_C(0x00)

#define IMPLICIT_HDR                    UINT8_C(0x01)
#define EXPLICIT_HDR                    UINT8_C(0x00)

#define CRC_ON                          UINT8_C(0x01)
#define CRC_OFF                         UINT8_C(0x00)

#define STD_IQ_SETUP                    UINT8_C(0x00)
#define INV_IQ_SETUP                    UINT8_C(0x00)

#define DIO2_AS_SW_CTRL                 UINT8_C(0x01)
#define DIO2_AS_IRQ                     UINT8_C(0x00)

#define DIO3_AS_TCXO_CTRL               UINT8_C(0x01)
#define DIO3_AS_IRQ                     UINT8_C(0x00)
#define TCXO_SETUP_TIME                 UINT32_C()

#define FS_FALLBACK_MODE                UINT8_C(0x40)
#define STBY_XOSC_FALLBACK_MODE         UINT8_C(0x30)
#define STBY_RC_FALLBACK_MODE           UINT8_C(0x20)

#define SX1262_DEV_TYPE                 UINT8_C(0x00)
#define SX1261_DEV_TYPE                 UINT8_C(0x01)

#define GFSK_PKT_TYPE                   UINT8_C(0x00)
#define LORA_PKT_TYPE                   UINT8_C(0x01)

#define XTAL_FREQ                       (double) 32000000
#define FREQ_DIV                        (double) pow(2.0, 25.0)
#define FREQ_STEP                       (double) (XTAL_FREQ/FREQ_DIV)

#define LORA_SYNC_WORD_PUB_NETWORK      UINT16_C(0x3444)
#define LORA_SYNC_WORD_PRI_NETWORK      UINT16_C(0x1424)

#define RX_GAIN_PWR_SAVING              UINT8_C(0x94)
#define RX_GAIN_BOOSTED                 UINT8_C(0x96)

#define OCP_SX1262_140_MA               UINT8_C(0x38)
#define OCP_SX1261_60_MA                UINT8_C(0x18)

#define LOW_DATA_RATE_MODE              UINT8_C(0x01)
#define NORM_DATA_RATE_MODE             UINT8_C(0x00)

/* Status */
// Masks
#define STATUS_CHIP_MODE_MASK           UINT8_C(0x70)
#define STATUS_CMD_STAT_MASK            UINT8_C(0x0E)

// Constants
#define STATUS_CHIP_MODE_STBY_RC        UINT8_C(0x02 << 4)
#define STATUS_CHIP_MODE_STBY_XOSC      UINT8_C(0x03 << 4)
#define STATUS_CHIP_MODE_FS             UINT8_C(0x04 << 4)
#define STATUS_CHIP_MODE_RX             UINT8_C(0x05 << 4)
#define STATUS_CHIP_MODE_TX             UINT8_C(0x06 << 4)

#define STATUS_CMD_STAT_DATA_AVAIL      UINT8_C(0x02 << 1)
#define STATUS_CMD_STAT_CMD_TIMEOUT     UINT8_C(0x03 << 1)
#define STATUS_CMD_STAT_CMD_PROC_ERR    UINT8_C(0x04 << 1)
#define STATUS_CMD_STAT_CMD_EXEC_ERR    UINT8_C(0x05 << 1)
#define STATUS_CMD_STAT_TX_DONE         UINT8_C(0x06 << 1)

/* TX Ouput Power Constants */
#define SET_RAMP_10U                    UINT8_C(0x00) // 10 us
#define SET_RAMP_20U                    UINT8_C(0x01) // 20 us
#define SET_RAMP_40U                    UINT8_C(0x02) // 40 us
#define SET_RAMP_80U                    UINT8_C(0x03) // 80 us
#define SET_RAMP_200U                   UINT8_C(0x04) // 200 us
#define SET_RAMP_800U                   UINT8_C(0x05) // 800 us
#define SET_RAMP_1700U                  UINT8_C(0x06) // 1700 us
#define SET_RAMP_3400U                  UINT8_C(0x07) // 3400 us

#define DIO3_TCXO_1V6                   UINT8_C(0x00)
#define DIO3_TCXO_1V7                   UINT8_C(0x01)
#define DIO3_TCXO_1V8                   UINT8_C(0x02)
#define DIO3_TCXO_2V2                   UINT8_C(0x03)
#define DIO3_TCXO_2V4                   UINT8_C(0x04)
#define DIO3_TCXO_2V7                   UINT8_C(0x05)
#define DIO3_TCXO_3V0                   UINT8_C(0x06)
#define DIO3_TCXO_3V3                   UINT8_C(0x07)

/* Masks */
/* IRQ Masks */
#define TX_DONE_MASK                    UINT16_C(0x1 << 0)
#define RX_DONE_MASK                    UINT16_C(0x1 << 1)
#define PREAMBLE_DETECTED_MASK          UINT16_C(0x1 << 2)
#define SYNC_WORD_VALID_MASK            UINT16_C(0x1 << 3)
#define HEADER_VALID_MASK               UINT16_C(0x1 << 4)
#define HEADER_ERR_MASK                 UINT16_C(0x1 << 5)
#define CRC_ERR_MASK                    UINT16_C(0x1 << 6)
#define CAD_DONE_MASK                   UINT16_C(0x1 << 7)
#define CAD_DETECTED_MASK               UINT16_C(0x1 << 8)
#define TIMEOUT_MASK                    UINT16_C(0x1 << 9)

// All enabled IRQ as default mask
#define DEFAULT_IRQ_MASK                UINT16_C(0x03FF);
#define DEFAULT_DIO1_IRQ_MASK           UINT16_C(0x03FF);
#define DEFAULT_DIO2_IRQ_MASK           UINT16_C(0x0000);
#define DEFAULT_DIO3_IRQ_MASK           UINT16_C(0x0000);

/* OP Error Masks */
#define RC64_CALIB_ERR_MASK             UINT16_C(0x0001)
#define RC13M_CALIB_ERR_MASK            UINT16_C(0x0001 << 1)
#define PLL_CALIB_ERR_MASK              UINT16_C(0x0001 << 2)
#define ADC_CALIB_ERR_MASK              UINT16_C(0x0001 << 3)
#define IMG_CALIB_ERR_MASK              UINT16_C(0x0001 << 4)
#define XOSC_START_ERR_MASK             UINT16_C(0x0001 << 5)
#define PLL_LOCK_ERR_MASK               UINT16_C(0x0001 << 6)
#define PA_RAMP_ERR_MASK                UINT16_C(0x0001 << 8)

/* LoRa Modulation Parameters */
typedef struct lora_modulation_parameters_t
{
    // RF Frequency
    uint32_t freq;

    // Modulation Parameter 1
    uint8_t spreading_factor;

    // Modulation Parameter 2
    uint8_t bandwidth;

    // Modulation Parameter 3
    uint8_t coding_rate;

    // Modulation Parameter 4
    uint8_t low_data_rate_opt;
} lora_modulation_params;

/* LoRa Packet Parameters */
typedef struct lora_packet_parameters_t
{
    uint16_t preamble_length;
    uint8_t header_type;
    uint8_t payload_length;
    uint8_t crc_type;
    uint8_t invert_iq;
} lora_packet_parameters;

// Spreading Factor Constants
#define LORA_SF_5                       UINT8_C(0x05) // Spreading Factor 5
#define LORA_SF_6                       UINT8_C(0x06) // Spreading Factor 6
#define LORA_SF_7                       UINT8_C(0x07) // Spreading Factor 7
#define LORA_SF_8                       UINT8_C(0x08) // Spreading Factor 8
#define LORA_SF_9                       UINT8_C(0x09) // Spreading Factor 9
#define LORA_SF_10                      UINT8_C(0x0A) // Spreading Factor 10
#define LORA_SF_11                      UINT8_C(0x0B) // Spreading Factor 11
#define LORA_SF_12                      UINT8_C(0x0C) // Spreading Factor 12

// Bandwidth Constants
#define LORA_BW_7                       UINT8_C(0x00) // 7.81 kHz
#define LORA_BW_10                      UINT8_C(0x08) // 10.32 kHz
#define LORA_BW_15                      UINT8_C(0x01) // 15.63 kHz
#define LORA_BW_20                      UINT8_C(0x09) // 20.83 kHz
#define LORA_BW_31                      UINT8_C(0x02) // 31.25 kHz
#define LORA_BW_41                      UINT8_C(0x0A) // 41.67 kHz
#define LORA_BW_62                      UINT8_C(0x03) // 62.50 kHz
#define LORA_BW_125                     UINT8_C(0x04) // 125 kHz
#define LORA_BW_250                     UINT8_C(0x05) // 250 kHz
#define LORA_BW_500                     UINT8_C(0x06) // 500 kHz

// Coding Rate Constants
#define LORA_CR_4_5                     UINT8_C(0x01)
#define LORA_CR_4_6                     UINT8_C(0x02)
#define LORA_CR_4_7                     UINT8_C(0x03)
#define LORA_CR_4_8                     UINT8_C(0x04)

// Low Data Rate Optimization Constants
#define LDR_OPT_OFF                     UINT8_C(0x00)
#define LDR_OPT_ON                      UINT8_C(0x01)

// LoRa Header Type Constants
#define PKT_EXPLICIT_HDR                UINT8_C(0x00) // Variable length packet
#define PKT_IMPLICIT_HDR                UINT8_C(0x01) // Fixed length packet

// LoRa CRC Type Constants
#define PKT_CRC_OFF                     UINT8_C(0x00)
#define PKT_CRC_ON                      UINT8_C(0x01)

// LoRa InvertIQ Constants
#define STANDARD_IRQ_SETUP              UINT8_C(0x00)
#define INVERTED_IRQ_SETUP              UINT8_C(0x01)

// CAD Number of Symbol Constants
#define CAD_ON_1_SYMB                   UINT8_C(0x00)
#define CAD_ON_2_SYMB                   UINT8_C(0x01)
#define CAD_ON_4_SYMB                   UINT8_C(0x02)
#define CAD_ON_8_SYMB                   UINT8_C(0x03)
#define CAD_ON_16_SYMB                  UINT8_C(0x04)

#define CAD_ONLY_EXIT                   UINT8_C(0x00)
#define CAD_RX_EXIT                     UINT8_C(0x01)

/* Packet Status */
typedef struct packet_status_t
{
    uint8_t rssi_pkt;
    int8_t snr_pkt;
    uint8_t sig_rssi_pkt;
} packet_status;

#define STAT_PREAMBLE_ERR_MASK          UINT8_C(0x01 << 7)
#define STAT_SYNC_ERR_MASK              UINT8_C(0x01 << 6)
#define STAT_SDRS_ERR_MASK              UINT8_C(0x01 << 5)
#define STAT_CRC_ERR_MASK               UINT8_C(0x01 << 4)
#define STAT_LEN_ERR_MASK               UINT8_C(0x01 << 3)
#define STAT_ABT_ERR_MASK               UINT8_C(0x01 << 2)
#define STAT_PKT_REC_MASK               UINT8_C(0x01 << 1)
#define STAT_PKT_SENT_MASK              UINT8_C(0x01)

/* Received Packet Statistics */
typedef struct packet_statistics_t
{
    uint16_t num_pkt_rx;
    uint16_t num_pkt_crc_err;
    uint16_t num_pkt_len_err;
} packet_stats;

//typedef struct sx_hardware_configuration_t
//{
    //spi_inst_t* spi_port;
    //uint8_t nss_pin;
    //uint8_t busy_pin;
    //uint8_t ant_sw_pwr_pin;

    //// IRQ Mask
    //uint16_t irq_mask;
    
    //// DIO1 Configuration
    //uint8_t dio1_pin;
    //uint16_t dio1_irq_mask;

    //// DIO2 Configuration
    //uint8_t dio2_config;    // RF SW CTRL or Not
    //uint8_t dio2_pin;       // If used as a general irq pin
    //uint16_t dio2_irq_mask;  // IRQ mask if used as a general irq pin

    //// DIO3 configuration
    //uint8_t dio3_config;    // DIO3 Used as TCXO ctrl
    //uint8_t tcxo_voltage;   // TCXO VDD voltage
    //uint32_t tcxo_config_delay; // TCXO configuration timeout
    //uint8_t dio3_pin;       // If used as gereral irq pin
    //uint16_t dio3_irq_mask;  // IRQ mask if used as a general irq pin

    //uint8_t dc_dc_ldo_mode;

//} sx_hw_config;

#define PACKET_BUFFER_SIZE 5

typedef struct packet_buffer_t
{
    uint8_t packets[PACKET_BUFFER_SIZE][256];
    uint8_t lengths[PACKET_BUFFER_SIZE];
} packet_buffer;

#endif
