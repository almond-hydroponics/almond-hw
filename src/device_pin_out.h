#ifndef DEVICE_PIN_OUT_H
#define DEVICE_PIN_OUT_H


#include <cstdint>

#include "device.h"

class Device_pin_out: public Device_output
{
public:
	Device_pin_out(const char *name, uint8_t pin, bool invert);
	void setup() override;
	void set_value(int value, bool verbose = false) override;

protected:
	uint8_t pin;
	bool invert;
};


#endif //DEVICE_PIN_OUT_H
