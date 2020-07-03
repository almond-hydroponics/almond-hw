#ifndef DEVICE_PIN_IN_H
#define DEVICE_PIN_IN_H


#include <cstdint>

#include "device.h"
#include "timer_override.h"

/** Simple filtered input, the value will be -1 in case of fiddling input and 0/1 when input is stable (for filter_len * 2 time when changing
 */


class Device_pin_in: public Device_input
{
public:
	static const int poll_interval = 100;

	Device_pin_in(const char *name,
				  uint8_t pin,
				  uint8_t filter_len,
				  bool invert);
	void setup() override;
	void loop() override;

protected:
	TimerOverride timer;
	int16_t filter_sum;
	uint8_t pin;
	uint8_t filter_len;
	bool invert;
};


#endif //DEVICE_PIN_IN_H
