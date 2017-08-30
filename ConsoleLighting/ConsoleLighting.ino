
#include <Arduino.h>
#include <stdint.h>
#include "LightDriver.h"

const char* FIRMWARE_VERSION = "Console Lighting v1.0 (compiled" __TIMESTAMP__ ")";

// Serial control chars
const char CMD_START_CHAR = '$',
	CMD_TERMINATOR_CHAR = '\n';

const uint8_t RX_BUFFER_SIZE = 20;
char rxBuffer[RX_BUFFER_SIZE];	// Not null-terminated
volatile uint8_t rxBufferIndex = 0;	// The next empty space in the rx buffer
volatile bool parseRequest = true;

LightDriver *driver;

void setup() {
	driver = new LightDriver(64);

	Serial.begin(115200);
	//while (!Serial);

	// Print start message
	Serial.println(FIRMWARE_VERSION);
}

void loop() {
	parseCommand();
	driver->updateOutputs();
}

void parseCommand() {
	// Do nothing if there is no command ready
	if (!parseRequest) {
		return;
	}

	// DEBUG Echo the command
	Serial.print("Command received: ");
	for (uint8_t i = 0; i < rxBufferIndex; i++) {
		Serial.print(rxBuffer[i]);
	}

	// TODO Parse

	// Clear the request
	parseRequest = false;
}

// Serial command structure: $
void serialEvent() {
	// Ignore if the previous command has not been parsed yet
	if (parseRequest) {
		return;
	}

	// Read from the port
	while (Serial.available()) {
		// Check that there is enough space in the buffer
		if (rxBufferIndex >= RX_BUFFER_SIZE) {
			// ERROR Buffer overflow
		}
		
		// Read a char from the port
		char c = (char)Serial.read();

		// Reset the buffer when a start char arrives
		if (c == CMD_START_CHAR) {
			rxBufferIndex = 0;
		}

		// Write the char to the buffer
		rxBuffer[rxBufferIndex++] = c;
		
		// Request parsing when newline is received
		if (c == CMD_TERMINATOR_CHAR) {
			parseRequest = true;
			break;
		}
	}
}
