/*
 * camera.c
 * OV2640
 */ 
#include "camera.h"
#include "ov2640.h"
#include "timer_interface.h"
#include <asf.h>

uint16_t g_us_cap_rows = IMAGE_HEIGHT; // set the number of rows in the captured image
uint16_t g_us_cap_line = (IMAGE_WIDTH * 2); // set the number of bytes in a single row of the captured image (multiplied by 2 since each pixel is represented by 2 bytes)
volatile uint32_t vsync_flag = false; // declare a volatile flag variable that indicates whether a vertical sync interrupt has occurred
volatile uint32_t image_ok_flag = false; // declare a volatile flag variable that indicates whether the captured image is valid
uint32_t volatile img_length = 0; // declare a volatile variable that holds the length of the captured image in bytes
uint32_t volatile sof_addr = 0; // declare a volatile variable that holds the address of the start of frame (SOF) marker in the captured image
uint32_t volatile eof_addr = 0; // declare a volatile variable that holds the address of the end of frame (EOF) marker in the captured image

void vsync_handler(uint32_t ul_id, uint32_t ul_mask) // define a function that will be called when a vertical sync interrupt occurs
{
	unused(ul_id); // ignore the unused parameter
	unused(ul_mask); // ignore the unused parameter

	vsync_flag = true; // set the flag variable to indicate that a vertical sync interrupt has occurred
}

/**
 * \brief Initialize Vsync_Handler.
 */
void init_vsync_interrupts(void) // define a function that initializes the vertical sync interrupt handler
{
	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(OV2640_VSYNC_PIO, OV2640_VSYNC_ID, OV2640_VSYNC_MASK,
			OV2640_VSYNC_TYPE, vsync_handler); // set up the PIO interrupt handler for the vertical sync pin and specify the vsync_handler function to be called when an interrupt occurs

	/* Enable PIO controller IRQs */
	NVIC_EnableIRQ((IRQn_Type)OV2640_VSYNC_ID); // enable the interrupt for the vertical sync pin
}

void configure_twi(void) // define a function that configures the two-wire interface (TWI) peripheral
{
	twi_options_t opt;
	
	/* Enable TWI peripheral */
	pmc_enable_periph_clk(ID_BOARD_TWI); // enable the peripheral clock for the TWI module

	/* Init TWI peripheral */
	opt.master_clk = sysclk_get_cpu_hz(); // set the TWI module's master clock to the CPU clock frequency
	opt.speed      = TWI_CLK; // set the TWI module's clock speed
	twi_master_init(BOARD_TWI, &opt); // initialize the TWI module with the specified options
	
	/* Configure TWI interrupts */
	NVIC_DisableIRQ(BOARD_TWI_IRQn); // disable the interrupt for the TWI module
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn); // clear any pending interrupts for the TWI module
	NVIC_SetPriority(BOARD_TWI_IRQn, 0); // set the priority of the TWI module's interrupt to the highest level (0)
	NVIC_EnableIRQ(BOARD_TWI_IRQn); // enable the interrupt for the TWI module
}

void pio_capture_init(Pio *p_pio, uint32_t ul_id)
{
	/* Enable peripheral clock */
	pmc_enable_periph_clk(ul_id);

	/* Disable pio capture */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	/* Disable rxbuff interrupt */
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	/* 32bit width*/
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);

	//#if !defined(DEFAULT_MODE_COLORED)
	/* Samples only data with even index */
	// p_pio->PIO_PCMR |= PIO_PCMR_HALFS;
	// p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_FRSTS);
	//#endif
}

// This function copies data from the PIO data register to a buffer.
// It returns 1 if the PIO is ready to transfer data, 0 if not.
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size)
{
	/* Check if the first PDC bank is free */
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)uc_buf;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
		} else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)uc_buf;
		p_pio->PIO_RNCR = ul_size;
		return 1;
		} else {
		return 0;
	}
}

// This function initializes the camera by configuring camera pins, camera clock (XCLK), and calling the configure twi function.
void init_camera(void)
{
	gpio_configure_pin(D0_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D1_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D2_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D3_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D4_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D5_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D6_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(D7_PIN, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(CAM_RST, OV_RST_FLAGS);
	gpio_configure_pin(HREF_PIN, OV_HSYNC_FLAGS);
	gpio_configure_pin(VSYNC_PIN, OV_VSYNC_FLAGS);
	gpio_configure_pin(SDA_PIN, TWI0_DATA_FLAGS);
	gpio_configure_pin(SCL_PIN, TWI0_CLK_FLAGS);
	
	pmc_enable_pllbck(7, 0x1, 1); // Enable the PLLB clock.
	gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS); // Configure the PCK1 pin.

	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK); // Set up the PCK1 clock.
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {
	}
	
	configure_twi();
}

// void configure camera(void): Configuration of OV2640 registers for desired operation.

void configure_camera(void) 
{
	init_vsync_interrupts();
	pio_capture_init(PIOA, ID_PIOA);
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_320x240);
}

// uint8 t start capture(void): Captures an image after a rising edge of VSYNC, and gets image length. Returns 1 on success (i.e. a nonzero image length), 0 on error.

uint8_t start_capture(void)
{
	/* Set cap_rows value*/
	g_us_cap_rows = IMAGE_HEIGHT;

	/* Enable vsync interrupt*/
	pio_enable_interrupt(OV2640_VSYNC_PIO, OV2640_VSYNC_MASK);

	/* Wait for rising edge of vsync signal before starting capture process */
	while (!vsync_flag) {
	}

	/* Disable vsync interrupt*/
	pio_disable_interrupt(OV2640_VSYNC_PIO, OV2640_VSYNC_MASK);

	/* Enable pio capture*/
	pio_capture_enable(OV2640_DATA_BUS_PIO);

	/* Capture data and send it to external SRAM memory using PDC feature */
	pio_capture_to_buffer(OV2640_DATA_BUS_PIO, cam_buffer,
	100000 >> 2);

	/* Wait for end of capture */
	while (!((OV2640_DATA_BUS_PIO->PIO_PCISR & PIO_PCIMR_RXBUFF) == PIO_PCIMR_RXBUFF)) {
	}

	/* Disable pio capture*/
	pio_capture_disable(OV2640_DATA_BUS_PIO);

	/* Reset vsync flag*/
	vsync_flag = false;

	/* Find image length based on JPEG protocol */
	find_image_len();
	
	return image_ok_flag;
}

// uint8 t find image len(void): Finds image length based on JPEG protocol. Returns 1 on success (i.e. able to find “end of image” and “start of image” markers), 0 on error.

void find_image_len(void)
{
	/* Initialize variables */
	uint8_t start_found = 0;
	uint8_t end_found = 0;
	uint32_t ii = 0;
	img_length = 0;
	
	/* Find start of image marker */
	while (!start_found) {
		if (cam_buffer[ii] == 0xFF && cam_buffer[ii+1] == 0xD8 && ii < BUF_SIZE) {
			sof_addr = ii;
			start_found = 1;
			} else {
			ii++;
		}
	}

	/* Find end of image marker */
	while (!(cam_buffer[ii] == 0xFF && cam_buffer[ii+1] == 0xD9) && ii < BUF_SIZE) {
		ii++;
		img_length++;
	}

	/* Calculate image length and set end of image address */
	img_length = img_length+2;
	eof_addr = ii+3;
	end_found = 1;
	image_ok_flag = true;
}