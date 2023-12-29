/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define BUF_SIZE 100000

// Camera Pins
#define D0_PIN PIO_PA24_IDX
#define D1_PIN PIO_PA25_IDX
#define D2_PIN PIO_PA26_IDX
#define D3_PIN PIO_PA27_IDX
#define D4_PIN PIO_PA28_IDX
#define D5_PIN PIO_PA29_IDX
#define D6_PIN PIO_PA30_IDX
#define D7_PIN PIO_PA31_IDX

#define HREF_PIN PIO_PA16_IDX
#define VSYNC_PIN PIO_PA15_IDX
#define CAM_XCLK PIO_PA17_IDX
#define CAM_RST PIO_PA20_IDX
#define SDA_PIN PIO_PA3_IDX
#define SCL_PIN PIO_PA4_IDX

#define PIN_PCK1 PIO_PA17_IDX
#define PIN_PCK1_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

// Wifi Pins
#define NETWORK_PIN PIO_PB11_IDX
#define CLIENTS_PIN PIO_PB13_IDX
#define WIFI_RESET_PIN PIO_PB0_IDX
#define COMM_PIN PIO_PB10_IDX

/** TWI0 data pin */
#define PIN_TWI_TWD0                   {PIO_PA3A_TWD0, PIOA, ID_PIOA, \
PIO_PERIPH_A, PIO_DEFAULT}

/** TWI0 clock pin */
#define PIN_TWI_TWCK0                  {PIO_PA4A_TWCK0, PIOA, ID_PIOA,	\
PIO_PERIPH_A, PIO_DEFAULT}

/** TWI0 Data pins definition */
#define TWI0_DATA_GPIO                 PIO_PA3_IDX
#define TWI0_DATA_FLAGS                (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_DATA_MASK                 PIO_PA3
#define TWI0_DATA_PIO                  PIOA
#define TWI0_DATA_ID                   ID_PIOA
#define TWI0_DATA_TYPE                 PIO_PERIPH_A
#define TWI0_DATA_ATTR                 PIO_DEFAULT

/** TWI0 clock pins definition */
#define TWI0_CLK_GPIO                  PIO_PA4_IDX
#define TWI0_CLK_FLAGS                 (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_CLK_MASK                  PIO_PA4
#define TWI0_CLK_PIO                   PIOA
#define TWI0_CLK_ID                    ID_PIOA
#define TWI0_CLK_TYPE                  PIO_PERIPH_A
#define TWI0_CLK_ATTR                  PIO_DEFAULT

/** TWI0 pins */
#define PINS_TWI0                      PIN_TWI_TWD0, PIN_TWI_TWCK0

#define ID_BOARD_TWI		               ID_TWI0
#define BOARD_TWI			                 TWI0
#define BOARD_TWI_IRQn		             TWI0_IRQn


/** pin for OV_VDD */
#define PIN_OV_SW_OVT                  { PIO_PC10, PIOC, ID_PIOC, \
	PIO_OUTPUT_1, PIO_DEFAULT} /* OV_VDD */
	#define PIN_OV_RST                     { PIO_PC20, PIOC, ID_PIOC, \
	PIO_OUTPUT_1, PIO_DEFAULT}
	#define PIN_OV_FSIN                    { PIO_PA21, PIOA, ID_PIOA, \
	PIO_OUTPUT_0, PIO_DEFAULT}

	/** HSYNC pin */
	#define PIN_OV_HSYNC                   { PIO_PA16, PIOA, ID_PIOA, \
		PIO_INPUT, PIO_PULLUP | \
	PIO_IT_RISE_EDGE }

	/** VSYNC pin */
	#define PIN_OV_VSYNC                   { PIO_PA15, PIOA, ID_PIOA, \
		PIO_INPUT, PIO_PULLUP | \
	PIO_IT_RISE_EDGE }

	/** OV_SW_OVT pin definition */
	#define OV_SW_OVT_GPIO                 PIO_PC10_IDX
	#define OV_SW_OVT_FLAGS                (PIO_OUTPUT_1 | PIO_DEFAULT)
	#define OV_SW_OVT_MASK                 PIO_PC10
	#define OV_SW_OVT_PIO                  PIOC
	#define OV_SW_OVT_ID                   ID_PIOC
	#define OV_SW_OVT_TYPE                 PIO_OUTPUT_1

	/** OV_RST pin definition */
	#define OV_RST_GPIO                    PIO_PA20_IDX
	#define OV_RST_FLAGS                   (PIO_OUTPUT_1 | PIO_DEFAULT)
	#define OV_RST_MASK                    PIO_PA20
	#define OV_RST_PIO                     PIOC
	#define OV_RST_ID                      ID_PIOC
	#define OV_RST_TYPE                    PIO_OUTPUT_1

	/** OV_RST pin definition */
	#define OV_FSIN_GPIO                   PIO_PA21_IDX
	#define OV_FSIN_FLAGS                  (PIO_OUTPUT_0 | PIO_DEFAULT)
	#define OV_FSIN_MASK                   PIO_PA21
	#define OV_FSIN_PIO                    PIOA
	#define OV_FSIN_ID                     ID_PIOA
	#define OV_FSIN_TYPE                   PIO_OUTPUT_0

	/** OV_HSYNC pin definition */
	#define OV_HSYNC_GPIO                  PIO_PA16_IDX
	#define OV_HSYNC_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
	#define OV_HSYNC_MASK                  PIO_PA16
	#define OV_HSYNC_PIO                   PIOA
	#define OV_HSYNC_ID                    ID_PIOA
	#define OV_HSYNC_TYPE                  PIO_PULLUP

	/** OV_VSYNC pin definition */
	#define OV_VSYNC_GPIO                  PIO_PA15_IDX
	#define OV_VSYNC_FLAGS                 (PIO_PULLUP | PIO_IT_RISE_EDGE)
	#define OV_VSYNC_MASK                  PIO_PA15
	#define OV_VSYNC_PIO                   PIOA
	#define OV_VSYNC_ID                    ID_PIOA
	#define OV_VSYNC_TYPE                  PIO_PULLUP

	/** OV Data Bus pins */
	#define OV_DATA_BUS_D2                 PIO_PA24_IDX
	#define OV_DATA_BUS_D3                 PIO_PA25_IDX
	#define OV_DATA_BUS_D4                 PIO_PA26_IDX
	#define OV_DATA_BUS_D5                 PIO_PA27_IDX
	#define OV_DATA_BUS_D6                 PIO_PA28_IDX
	#define OV_DATA_BUS_D7                 PIO_PA29_IDX
	#define OV_DATA_BUS_D8                 PIO_PA30_IDX
	#define OV_DATA_BUS_D9                 PIO_PA31_IDX
	#define OV_DATA_BUS_FLAGS              (PIO_INPUT | PIO_PULLUP)
	#define OV_DATA_BUS_MASK               (0xFF000000UL)
	#define OV_DATA_BUS_PIO                PIOA
	#define OV_DATA_BUS_ID                 ID_PIOA
	#define OV_DATA_BUS_TYPE               PIO_INPUT
	#define OV_DATA_BUS_ATTR               PIO_DEFAULT

	/** List of Image sensor definitions. */
	#define PINS_OV                        PIN_OV_SW_OVT, PIN_PCK0, PIN_OV_RST


#define ID_BOARD_TWI		               ID_TWI0
#define BOARD_TWI			                 TWI0
#define BOARD_TWI_IRQn		             TWI0_IRQn

/******************************* TWI definition
 *********************************/
/** TWI0 data pin */
#define PIN_TWI_TWD0                   {PIO_PA3A_TWD0, PIOA, ID_PIOA, \
                                         PIO_PERIPH_A, PIO_DEFAULT}

/** TWI0 clock pin */
#define PIN_TWI_TWCK0                  {PIO_PA4A_TWCK0, PIOA, ID_PIOA,	\
                                         PIO_PERIPH_A, PIO_DEFAULT}

/** TWI0 Data pins definition */
#define TWI0_DATA_GPIO                 PIO_PA3_IDX
#define TWI0_DATA_FLAGS                (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_DATA_MASK                 PIO_PA3
#define TWI0_DATA_PIO                  PIOA
#define TWI0_DATA_ID                   ID_PIOA
#define TWI0_DATA_TYPE                 PIO_PERIPH_A
#define TWI0_DATA_ATTR                 PIO_DEFAULT

// Image sensor VSYNC pin.
#define OV2640_VSYNC_PIO			   OV_VSYNC_PIO
#define OV2640_VSYNC_ID				   OV_VSYNC_ID
#define OV2640_VSYNC_MASK              OV_VSYNC_MASK
#define OV2640_VSYNC_TYPE              OV_VSYNC_TYPE

// Image sensor data pin.
#define OV2640_DATA_BUS_PIO            OV_DATA_BUS_PIO
#define OV2640_DATA_BUS_ID             OV_DATA_BUS_ID

#define OV_DATA_BUS_PIO                PIOA

#endif // CONF_BOARD_H
