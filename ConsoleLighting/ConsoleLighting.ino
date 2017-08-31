
#include <stdint.h>
#include "LightDriver.h"

const char* const FIRMWARE_VERSION = "Console Lighting v1.0 (compiled " __TIMESTAMP__ ")";

const char* const ACK_COMMAND_ERROR = "Command error.";

// Serial control chars
const char
CMD_START_CHAR = '$',
CMD_ASSIGNMENT_CHAR = '=',
CMD_TERMINATOR_CHAR = '\n';

const uint8_t RX_BUFFER_SIZE = 20;	// The maximum size of a message to be parsed
char rxBuffer[RX_BUFFER_SIZE];		// Not null-terminated
volatile uint8_t rxBufferIndex = 0;	// The next empty space in the rx buffer
volatile bool parseRequest = false;

LightDriver *lightDriver;

//uint32_t loopTime = 0;
//uint32_t nowMicros = 0;

void setup() {
	// Open the serial port
	Serial.begin(115200);
	while (!Serial);	// Wait for connection on boards that support it

	// Print start message
	Serial.println(FIRMWARE_VERSION);

	// Create light driver
	const uint8_t BOARDS = 2;
	lightDriver = new LightDriver(32 * BOARDS);

	// Lamp test
	// TODO Maybe do this sequentially to reduce load
	lightDriver->setAllLightModes(LIGHT_MODE_ON);
	lightDriver->updateOutputs();
	delay(1000);
	lightDriver->setAllLightModes(LIGHT_MODE_OFF);
	lightDriver->updateOutputs();
}

void loop() {
	parseCommand();
	lightDriver->updateOutputs();

	// DEBUG
	/*static int counter = 0;
	nowMicros = micros();
	if (++counter >= 1000 && Serial.availableForWrite()) {
		Serial.println(nowMicros - loopTime);
		counter = 0;
	}
	loopTime = nowMicros;*/
}

void parseCommand() {
	// Do nothing if there is no command ready
	if (!parseRequest) {
		return;
	}

	// DEBUG Echo the command
	Serial.print(F("Command received: "));
	for (uint8_t i = 0; i < rxBufferIndex; i++) {
		Serial.print(rxBuffer[i]);
	}

	// Parse the command
	if (rxBuffer[1] == 'L') {	// Light commands
		if (rxBuffer[2] == 'M') {	// Mode
			if (rxBuffer[3] != '?') {
				// Set light mode command
				uint8_t assignmentIndex = rxBufferIndexOf(CMD_ASSIGNMENT_CHAR);	// The index of the assignment operator in the string
				if (assignmentIndex == -1 || (assignmentIndex + 1) >= rxBufferIndex) {
					// The assignment operator was not found
					Serial.println(ACK_COMMAND_ERROR);
				} else {
					// Assignment operator found
					char lightIndexChar = rxBuffer[3];
					if (isDigit(lightIndexChar)) {
						// There is a channel number
						uint8_t lightChannel = atoi(rxBuffer + 3);
						LightMode lightMode = (LightMode)atoi(rxBuffer + assignmentIndex + 1);
						lightDriver->setLightMode(lightChannel, lightMode);
					} else if (lightIndexChar == 'A') {
						// Set all channels
						LightMode lightMode = (LightMode)atoi(rxBuffer + assignmentIndex + 1);
						lightDriver->setAllLightModes(lightMode);
					} else {
						// Invalid channel
						Serial.println(ACK_COMMAND_ERROR);
					}
				}
			} else {
				// $LM help
				Serial.println(F("$LM<channel>=<mode>\tSet light mode"));
				Serial.print(F("<channel> is a number between 0 and "));
				Serial.print(lightDriver->getLightCount() - 1);
				Serial.println(F(", or \'A\' to set all channels."));
				Serial.print(F("<mode> is a number between 0 and "));
				Serial.print(LightMode_Length - 1);
				Serial.println(F(":"));
				for (uint8_t mode; mode < LightMode_Length; mode++) {
					Serial.print(mode);
					Serial.print(F("\t"));
					Serial.println(LightDriver::lightModeToString((LightMode)mode));
				}
			}
		} else {
			// Light command help
			Serial.println(F("Enter for more information:"));
			Serial.println(F("$LM?\tSet light mode"));
		}
	} else {
		// Command help
		Serial.println(F("Enter for more information:"));
		Serial.println(F("$L?\tLight commands"));
	}

	// Clear the request
	clearParseRequest();
}

void clearParseRequest() {
	rxBufferIndex = 0;
	parseRequest = false;
}

void serialEvent() {
	// Ignore if the previous command has not been parsed yet
	if (parseRequest) {
		return;
	}

	// Read from the port
	while (Serial.available()) {
		// Check that there is enough space in the buffer
		if (rxBufferIndex >= RX_BUFFER_SIZE) {
			Serial.println("ERROR: rx buffer overflow");
			rxBufferIndex = 0;	// Empty the buffer
			return;
		}

		// Read a char from the port
		char c = (char)toUpperCase(Serial.read());

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

int rxBufferIndexOf(char c) {
	for (uint8_t i = 0; i < rxBufferIndex; i++) {
		if (rxBuffer[i] == c) {
			return i;
		}
	}
	return -1;
}
