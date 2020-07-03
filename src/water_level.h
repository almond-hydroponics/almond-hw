#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <cstdint>

#include "device.h"
#include "timer_override.h"


class Water_level : public Device_input
{
public:
	static const int nfail_limit = 16;
	static const int measure_interval = 10000;

	Water_level(const char *name, uint8_t pin_trigger, uint8_t pin_echo);
	void loop() override;
	void setup() override;

protected:
	void measure_pulse_in();

	TimerOverride timer;
	uint8_t pin_trigger;
	uint8_t pin_echo;
	uint8_t nfails;
};


#endif //WATER_LEVEL_H
