/*
 * wifi.c
 * ESP32
 */ 
#include <wifi.h>
#include <camera.h>
#include <stdio.h>
#include <string.h>
#include <timer_interface.h>
#include <asf.h>

volatile uint32_t received_byte_wifi = 0; // holds the received byte from wifi
volatile bool new_rx_wifi = false; // flag to indicate whether there is a new received byte from wifi
volatile uint32_t input_pos_wifi = 0; // current position in the input buffer for wifi
volatile bool usart_comm_flag = false; // flag to indicate whether there is successful USART communication between mcu and wifi
volatile bool test_success_flag = false; // flag to indicate whether test was successful

volatile uint32_t transfer_index = 0; // current index for transferring data
volatile uint32_t transfer_len = 0; // current length for transferring data

volatile uint32_t button_flag = false; // flag for button push
volatile uint32_t counter = 0; // counter for button push
volatile bool provision_flag = false; // flag to indicate whether wifi provisioning is complete

// interrupt handler for USART communication with wifi
void wifi_usart_handler(void)
{
	uint32_t ul_status;
	
	ul_status = usart_get_status(WIFI_USART); // get the status of the USART
	
	// check if receive buffer is full
	if (ul_status & US_CSR_RXBUFF) {
		usart_read(WIFI_USART, &received_byte_wifi); // read the received byte from the USART
		new_rx_wifi = true; // set the new received byte flag to true
		process_incoming_byte_wifi((uint8_t)received_byte_wifi); // process the incoming byte
	}
}

// function to process incoming byte from wifi
void process_incoming_byte_wifi(uint8_t in_byte)
{
	input_line_wifi[input_pos_wifi++] = in_byte; // store the incoming byte in the input buffer
}

// interrupt handler for wifi command response
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask)
{
	process_data_wifi(); // process the wifi data
	for (uint32_t jj = 0; jj < 1000; jj++) { // reset the input buffer
		input_line_wifi[jj] = 0;
	}
	input_pos_wifi = 0; // reset the input position in the input buffer
}

// function to process wifi data
void process_data_wifi(void)
{
	usart_comm_flag = true; // set the USART communication flag to true
	if (strstr(input_line_wifi, "SUCCESS")) { // check if the input buffer contains "SUCCESS"
		test_success_flag = true; // set the test success flag to true
	} // may need to add more code to handle other data from the wifi buffer
}

// interrupt handler for wifi provisioning
void wifi_provision_handler(uint32_t ul_id, uint32_t ul_mask)
{
	unused(ul_id);
	unused(ul_mask);

	provision_flag = true; // set the wifi provisioning flag to true
	
	counter++; // increment the button push counter
	// delay_ms(1000); // delay for 1 second
}

// Handler for peripheral mode interrupts on SPI bus
void wifi_spi_handler(void)
{
	uint32_t new_cmd = 0;
	static uint16_t data;	// static variable to store received data from SPI
	uint8_t uc_pcs;

	if (spi_read_status(SPI) && SPI_SR_RDRF) {	// Check if SPI has received data
		spi_read(SPI, &data, &uc_pcs);	// Read received data from SPI
		
		if (transfer_len--) {	// Decrement transfer length and check if transfer is still in progress
			spi_write(SPI, cam_buffer[transfer_index++], 0, 0);	// Write image data to SPI
		}
	}
}

void configure_usart_wifi(void)
{
	// Configure USART pins
	gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS); 

	// Set USART configuration settings
	const sam_usart_opt_t usart_console_settings = {
		WIFI_USART_BAUDRATE,	// USART baud rate
		US_MR_CHRL_8_BIT,	// 8-bit character length
		US_MR_PAR_NO,		// No parity
		US_MR_NBSTOP_1_BIT,	// 1 stop bit
		US_MR_CHMODE_NORMAL,	// Normal mode
		0			// Not used in IrDA mode
	};

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(WIFI_ID_USART);

	/* Configure USART in serial mode. */
	usart_init_rs232(WIFI_USART, &usart_console_settings, sysclk_get_peripheral_hz());

	/* Disable all the interrupts. */
	usart_disable_interrupt(WIFI_USART, ALL_INTERRUPT_MASK);

	/* Enable the receiver and transmitter. */
	usart_enable_tx(WIFI_USART);
	usart_enable_rx(WIFI_USART);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(WIFI_USART_IRQn);	// Enable USART interrupt
	
	usart_enable_interrupt(WIFI_USART, US_IER_RXRDY);	// Enable receiver ready interrupt
}

void configure_wifi_comm_pin(void)
{
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WIFI_COMM_ID);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_NUM, WIFI_COMM_ATTR, wifi_command_response_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_NUM);
}

void configure_wifi_provision_pin(void)
{
	/* Configure PIO clock. */
	pmc_enable_periph_clk(PROV_PUSH_BUTTON_ID);

	/* Adjust PIO debounce filter using a 10 Hz filter. */
	pio_set_debounce_filter(PROV_PUSH_BUTTON_PIO, PROV_PUSH_BUTTON_PIN_MSK, 10);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(PROV_PUSH_BUTTON_PIO, PROV_PUSH_BUTTON_ID, PROV_PUSH_BUTTON_PIN_MSK,
			PROV_PUSH_BUTTON_ATTR, wifi_provision_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)PROV_PUSH_BUTTON_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(PROV_PUSH_BUTTON_PIO, PROV_PUSH_BUTTON_PIN_MSK);
}

// Configures the SPI pins and interrupts for slave mode.
void configure_spi(void)
{
	// Configure the MISO, MOSI, SPCK, and NPCS0 pins.
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	gpio_configure_pin(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
	
	/* Configure SPI interrupts for slave only. */
	// Disable and clear any pending interrupts on the SPI peripheral.
	NVIC_DisableIRQ(SPI_IRQn);
	NVIC_ClearPendingIRQ(SPI_IRQn);
	// Set the priority of the SPI interrupt to the highest level (0).
	NVIC_SetPriority(SPI_IRQn, 0);
	// Enable the SPI interrupt.
	NVIC_EnableIRQ(SPI_IRQn);
}

// Initializes the SPI peripheral for slave mode with specified settings.
void spi_peripheral_initialize(void)
{
	// Enable the clock for the SPI peripheral.
	spi_enable_clock(SPI);
	// Disable the SPI peripheral before resetting it.
	spi_disable(SPI);
	// Reset the SPI peripheral.
	spi_reset(SPI);
	// Set the SPI peripheral to slave mode.
	spi_set_slave_mode(SPI);
	// Disable mode fault detection.
	spi_disable_mode_fault_detect(SPI);
	// Set the chip select value for the SPI peripheral.
	spi_set_peripheral_chip_select_value(SPI, SPI_CHIP_PCS);
	// Set the clock polarity and phase for the SPI peripheral.
	spi_set_clock_polarity(SPI, SPI_CHIP_SEL, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI, SPI_CHIP_SEL, SPI_CLK_PHASE);
	// Set the number of bits per transfer for the SPI peripheral.
	spi_set_bits_per_transfer(SPI, SPI_CHIP_SEL, SPI_CSR_BITS_8_BIT);
	// Enable interrupts on the SPI peripheral for receive data ready.
	spi_enable_interrupt(SPI, SPI_IER_RDRF);
	// Enable the SPI peripheral.
	spi_enable(SPI);
}

// Prepares the SPI transfer by setting the transfer length and index.
void prepare_spi_transfer(void)
{
	transfer_len = img_length;
	transfer_index = sof_addr;
}

// Sends a WiFi command over USART.
void write_wifi_command(char * comm, uint8_t cnt)
{
	counts = 0;
	// Write the command to the WiFi USART.
	usart_write_line(WIFI_USART, comm);
	// Reset the USART communication flag.
	usart_comm_flag = false;

	// Wait for the USART communication flag to be set or for the specified number of counts to occur.
	while (!usart_comm_flag && counts < cnt) {
		
	}
	return;
}

// Writes the image data to the web server using SPI and WiFi.
void write_image_to_web(void)
{
	// Prepare the SPI transfer by setting the transfer length and index.
	prepare_spi_transfer();
	
	// Send a WiFi command to the server with the image length.
	char comm_buf[100];
	sprintf(comm_buf, "image_test %u\r\n", img_length);
	write_wifi_command(comm_buf,19);
}