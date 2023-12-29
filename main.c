#include <asf.h>
#include <camera.h>
#include <wifi.h>
#include <ov2640.h>
#include <timer_interface.h>

int main (void)
{
	
	// Initialize clock and board definitions
	sysclk_init();
	wdt_disable(WDT); //enable when running production code, disable for debugging
	board_init();
	ioport_init();
	
	// Initialize the camera
	init_camera();
	configure_camera();

	configure_tc(); // Configure the timer
	
	// Configure the WiFi module
	// configure_wifi_provision_pin(); // Optional: Provisioning pin
	configure_wifi_comm_pin(); // Communication pin
	configure_usart_wifi(); // USART interface
	configure_spi(); // SPI interface
	spi_peripheral_initialize(); // Initialize SPI peripheral

	// Send various commands to configure the WiFi module
	write_wifi_command("set wlan_gpio 25\r\n", 5);
	write_wifi_command("set websocket_gpio 26\r\n", 5);
	write_wifi_command("set ap_gpio 27\r\n", 5);
	write_wifi_command("set net_gpio 22\r\n", 5);
	write_wifi_command("set clients_gpio 23\r\n", 5);
	write_wifi_command("set comm_gpio 21\r\n", 5);
	write_wifi_command("set spi_baud 100000\r\n", 5);
	
	write_wifi_command("test\r\n", 10); // Test the WiFi module for successful initialization
	while (!test_success_flag) { // Wait until the WiFi module is successfully initialized
	}

	// Reset the WiFi module
	ioport_set_pin_dir(WIFI_RESET_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(WIFI_RESET_PIN, false);
	delay_ms(200);
	ioport_set_pin_level(WIFI_RESET_PIN, true);
	
	// Send a provisioning command to the WiFi module and wait for the provisioning to complete
	write_wifi_command("provision\r\n", 5);
	delay_ms(5000);
	while (!ioport_get_pin_level(NETWORK_PIN)&& !ioport_get_pin_level(CLIENTS_PIN)) {
		if (provision_flag) { // Optional: Handle provisioning flag
		}
	}
	
	while (true) {
		start_capture(); // Start capturing images using the camera module

		// If network is available and clients are connected, transfer the image to the ESP32 module
		if (ioport_get_pin_level(NETWORK_PIN) && ioport_get_pin_level(CLIENTS_PIN)) {
			// If picture taken successfully, transfer it to the ESP32 using the WiFi module
			write_image_to_web(); // Call function to write the image to the ESP32 module
		}
	}
}