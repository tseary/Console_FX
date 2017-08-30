
#pragma once

enum LightSource {
	SOURCE_OFF,
	SOURCE_ON,
	SOURCE_BLINK_FAST,
	SOURCE_BLINK_SLOW
};

class LightDriver {
public:
	LightDriver(uint8_t lightChannels) {
		// Set up light fields
		_lightChannels = lightChannels;
		_lightSources = new LightSource[_lightChannels];
		_lightOutputs = new uint8_t[(_lightChannels + 7) / 8];

		// TODO Set up pins
		//pinMode();
	}
	~LightDriver() {
		delete[] _lightOutputs;
	}

protected:
	uint8_t _lightChannels;		// The number of outputs connected
	LightSource *_lightSources;	// The function each light is performing
	uint8_t *_lightOutputs;		// One bit for each light, representing whether the light is currently on or off

	// TODO Pins
	//uint8_t _

	// Blink timing in millis
	uint16_t
		_blinkFastPeriod = 333,
		_blinkSlowPeriod = 1000;

	void updateOutputs() {
		// Determine whether each light source is currently on or off
		uint32_t nowMillis = millis();
		bool blinkFastOn = (nowMillis % _blinkFastPeriod) < (_blinkFastPeriod / 2);
		bool blinkSlowOn = (nowMillis % _blinkSlowPeriod) < (_blinkSlowPeriod / 2);

		// TODO For each light, set or clear a bit in _lightOutputs
		// TODO Shift out _lightOutputs (only if something changed)
	}
};
