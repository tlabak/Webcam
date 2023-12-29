/*
 * camera.h
 *
 */  
#include <asf.h>

#ifndef CAMERA_H_
#define CAMERA_H_

#define IMAGE_HEIGHT 240UL
#define IMAGE_WIDTH 320UL

/* SRAM board defines. */

/* TWI clock frequency in Hz (400KHz) */
// CHANGE
#define TWI_CLK     (400000UL)

/* Pointer to the image data destination buffer */
uint8_t cam_buffer[BUF_SIZE];
uint32_t volatile img_length;
uint32_t volatile sof_addr;
uint32_t volatile eof_addr;
volatile uint32_t vsync_flag;

#endif

void vsync_handler(uint32_t ul_id, uint32_t ul_mask);
void init_vsync_interrupts(void);
void configure_twi(void);
void init_camera(void);
void configure_camera(void);
void pio_capture_init(Pio *p_pio, uint32_t ul_id);
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);
uint8_t start_capture(void);
void find_image_len(void);
