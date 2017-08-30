
#pragma once

#include <Arduino.h>
#include <stdint.h>

enum LightMode {
	LIGHT_MODE_OFF,
	LIGHT_MODE_ON,
	LIGHT_MODE_FLASH_FAST,
	LIGHT_MODE_FLASH_SLOW,
	LIGHT_MODE_FLASH_FLASH_DOUBLE_FLASH
};

class LightDriver {
public:
	LightDriver(uint8_t lightCount) {
		// Set up light fields
		_lightCount = lightCount;
		_lightModes = new LightMode[lightCount];
		_lightOutputsLength = (lightCount + 7) / 8;
		_lightOutputs = new uint8_t[_lightOutputsLength];

		// Set up pins
		pinMode(_outputEnablePin, OUTPUT);
		pinMode(_shiftRegClearPin, OUTPUT);
		pinMode(_shiftRegDataPin, OUTPUT);
		pinMode(_shiftRegClockPin, OUTPUT);
		pinMode(_outputRegClockPin, OUTPUT);

		digitalWrite(_shiftRegDataPin, LOW);	// Data 0
		digitalWrite(_shiftRegClockPin, LOW);	// Shift clock inactive
		digitalWrite(_outputRegClockPin, LOW);	// Output clock inactive

		// Clear the registers
		reset();
		setAllLightModes(LIGHT_MODE_OFF);
	}
	~LightDriver() {
		delete[] _lightOutputs;
	}

	// Clears the shift and output registers and enables the outputs.
	// Does not change light modes that have been set.
	void reset();

	// Sets the mode of a single light.
	void setLightMode(uint8_t index, LightMode mode);

	// Sets the mode of all lights.
	void setAllLightModes(LightMode mode);

	// Updates all light outputs. Call repeatedly to drive animations.
	void updateOutputs();

	// Gets a human-readable string representing the given light mode.
	static const char* lightModeToString(LightMode lightMode);

protected:
	// Pins
	const uint8_t
		_outputEnablePin = 3,	// PD3, Active low
		_shiftRegClearPin = 6,	// PD6, L = shift register cleared
		_shiftRegDataPin = 2,	// PD2, Data input to shift register
		_shiftRegClockPin = 5,	// PD5, Rising edge = Value at data pin loaded into shift register
		_outputRegClockPin = 4;	// PD4, Rising edge = Data in shift register moves to output

	// Blink timing in millis
	uint16_t
		_flashFastPeriod = 333,
		_flashSlowPeriod = 1000;

	// Lighting control fields
	uint8_t _lightCount;		// The number of outputs connected
	LightMode *_lightModes;	// The function each light is performing
	uint8_t _lightOutputsLength;
	uint8_t *_lightOutputs;		// One bit for each light, representing whether the light is currently on or off

	void loadOutputRegister() {
		digitalWrite(_outputRegClockPin, HIGH);
		delay(1);
		digitalWrite(_outputRegClockPin, LOW);
	}
};
