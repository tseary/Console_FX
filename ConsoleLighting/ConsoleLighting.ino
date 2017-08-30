
#include <Arduino.h>
#include <stdint.h>
#include "LightDriver.h"

const uint8_t RX_BUFFER_SIZE = 20;
const char rxBuffer[RX_BUFFER_SIZE];	// Not null-terminated
uint8_t rxBufferIndex = 0;	// The next empty space in the rx buffer

LightDriver *driver;

void setup() {
	driver = new LightDriver(64);

	Serial.begin(115200);
}

void loop() {
	// TODO Service serial events
	driver->updateOutputs();
}

void serialEvent() {
	size_t availableData = RX_BUFFER_SIZE - rxBufferIndex;
	size_t bytesRead = Serial.readBytesUntil('\n', (char*)rxBuffer, availableData);
	rxBufferIndex += bytesRead;
}
