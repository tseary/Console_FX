
#include "LightDriver.h"

void LightDriver::reset() {
	// Clear the shift register
	digitalWrite(_shiftRegClearPin, LOW);
	delay(1);
	digitalWrite(_shiftRegClearPin, HIGH);

	// Clear the output register
	loadOutputRegister();

	// Enable outputs
	digitalWrite(_outputEnablePin, LOW);
}

void LightDriver::setLightMode(uint8_t index, LightMode mode) {
	// Ignore if the index is out of range
	if (index >= _lightCount) {
		return;
	}

	// Set the mode
	_lightModes[index] = mode;

	// DEBUG
	Serial.print("Set light ");
	Serial.print(index);
	Serial.print(" mode: ");
	Serial.print(mode);
	Serial.print(" ");
	Serial.println(lightModeToString(mode));
}

void LightDriver::setAllLightModes(LightMode mode) {
	// Set all lights to the given mode
	for (uint8_t i = 0; i < _lightCount; i++) {
		_lightModes[i] = mode;
	}

	// DEBUG
	Serial.print("Set all light modes: ");
	Serial.print(mode);
	Serial.print(" ");
	Serial.println(lightModeToString(mode));
}

void LightDriver::updateOutputs() {
	// Determine whether each light mode is currently on or off
	uint32_t nowMillis = millis();
	bool flashFast = (nowMillis % _flashFastPeriod) < (_flashFastPeriod / 2);
	bool flashSlow = (nowMillis % _flashSlowPeriod) < (_flashSlowPeriod / 2);
	bool flashFlashDoubleFlash = false;	// TODO

	// Generate outputs[] from lightModes[]
	uint8_t outputByte = 0;
	bool change = false;	// True if any lights have changed state

	for (uint8_t i = 0; i < _lightCount; i++) {
		// The position of the current bit
		uint8_t bitIndex = i % 8;

		// Clear at the start of each output byte, or shift the previous bit
		if (bitIndex == 0) {
			outputByte = 0x00;
		} else {
			outputByte <<= 1;
		}

		// Set the lowest bit if the light should be on
		switch (_lightModes[i]) {
			default:
			case LIGHT_MODE_OFF:
				break;

			case LIGHT_MODE_ON:
				outputByte |= 0x01;
				break;

			case LIGHT_MODE_FLASH_FAST:
				if (flashFast) {
					outputByte |= 0x01;
				}
				break;

			case LIGHT_MODE_FLASH_SLOW:
				if (flashSlow) {
					outputByte |= 0x01;
				}
				break;

			case LIGHT_MODE_FLASH_FLASH_DOUBLE_FLASH:
				if (flashFlashDoubleFlash) {
					outputByte |= 0x01;
				}
				break;
		}

		// Store output byte in array at end of byte or last light
		if (bitIndex == 7 || i == (_lightCount - 1)) {
			uint8_t byteIndex = i / 8;
			change |= _lightOutputs[byteIndex] != outputByte;
			_lightOutputs[byteIndex] = outputByte;
		}
	}

	// Only update registers if there is a change
	if (change) {
		// Shift out all bytes
		for (int b = _lightOutputsLength - 1; b >= 0; b--) {
			shiftOut(_shiftRegDataPin, _shiftRegClockPin, LSBFIRST, _lightOutputs[b]);
		}

		// Load all data to output register
		loadOutputRegister();
	}
}

const char* LightDriver::lightModeToString(LightMode lightMode) {
	switch (lightMode) {
		default:
		case LIGHT_MODE_OFF:
			return "Off";
		case LIGHT_MODE_ON:
			return "On";
		case LIGHT_MODE_FLASH_FAST:
			return "Flash Fast";
		case LIGHT_MODE_FLASH_SLOW:
			return "Flash Slow";
		case LIGHT_MODE_FLASH_FLASH_DOUBLE_FLASH:
			return "Flash... Flash... Double Flash";
	}
}
