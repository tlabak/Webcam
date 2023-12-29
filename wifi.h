/*
 * wifi.h
 *
 */ 
#ifndef WIFI_H_
#define WIFI_H_

#include <asf.h>
#include <string.h>

volatile char input_line_wifi[1000];
volatile uint32_t input_pos_wifi;

volatile bool provision_flag;
volatile bool usart_comm_flag;
volatile bool test_success_flag;


// USART DEFINITIONS // 
#define WIFI_ID_USART               ID_USART0
#define WIFI_USART                  USART0
#define WIFI_USART_BAUDRATE         115200
#define WIFI_USART_IRQn			    USART0_IRQn
#define wifi_usart_handler          USART0_Handler
#define wifi_spi_handler            SPI_Handler

#define LED_PIN		PIO_PA20_IDX

#define ALL_INTERRUPT_MASK  0xffffffff

/** USART0 pin RX */
#define PIN_USART0_RXD	            {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_RXD_IDX          (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS        (PIO_PERIPH_A | PIO_PULLUP)
/** USART0 pin TX */
#define PIN_USART0_TXD              {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_TXD_IDX          (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS        (PIO_PERIPH_A | PIO_PULLUP)

#define WIFI_COMM_PIN_NUM			PIO_PB10
#define WIFI_COMM_PIO				PIOB
#define WIFI_COMM_ID				ID_PIOB
#define WIFI_COMM_MASK				PIO_PB10_IDX
#define WIFI_COMM_ATTR				PIO_IT_RISE_EDGE


// PROVISIONING PIN/BUTTON DEFINITIONS //
#define PROV_PUSH_BUTTON_PIN_MSK       PIO_PA22
#define PROV_PUSH_BUTTON_PIO           PIOA
#define PROV_PUSH_BUTTON_ID            ID_PIOA
#define PROV_PUSH_BUTTON_ATTR          PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE

volatile uint32_t counter;
volatile uint32_t button_flag;


// SPI DEFINITIONS //
/** SPI MISO pin definition. */
#define SPI_MISO_GPIO        (PIO_PA12_IDX)
#define SPI_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO        (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO        (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

/** SPI chip select 0 pin definition. (Only one configuration is possible) */
#define SPI_NPCS0_GPIO         (PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS        (PIO_PERIPH_A | PIO_DEFAULT)

/* Chip select. */
#define SPI_CHIP_SEL 0
#define SPI_CHIP_PCS spi_get_pcs(SPI_CHIP_SEL)

/* Clock polarity. */
#define SPI_CLK_POLARITY 0

/* Clock phase. */
#define SPI_CLK_PHASE 0//1

/* Delay before SPCK. */
#define SPI_DLYBS 0x40

/* Delay between consecutive transfers. */
#define SPI_DLYBCT 0x10

volatile uint32_t transfer_index;
volatile uint32_t transfer_len;

void wifi_usart_handler(void);
void process_incoming_byte_wifi(uint8_t in_byte);
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
void process_data_wifi(void);
void wifi_provision_handler(uint32_t ul_id, uint32_t ul_mask);
void wifi_spi_handler(void);
void configure_usart_wifi(void);
void configure_wifi_comm_pin(void);
void configure_wifi_provision_pin(void);
void configure_spi(void);
void spi_peripheral_initialize(void);
void prepare_spi_transfer(void);
void write_wifi_command(char* comm, uint8_t cnt);
void write_image_to_web(void);

#endif



