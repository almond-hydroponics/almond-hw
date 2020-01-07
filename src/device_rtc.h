#ifndef DEVICE_RTC_H
#define DEVICE_RTC_H

#include <cstdint>

#include "device.h"
#include "RTClib.h"
#include "timer_override.h"
#include "config.h"

class Device_rtc: public Device_input
{
public:
	explicit Device_rtc(const char *name);
	void loop() override;
	void setup() override;
	void update_time(uint32_t ntp_time);
	void time_of_day(Config_run_table_time *time);

protected:
	RTC_DS3231 rtc;
	TimerOverride timer;
};


#endif //DEVICE_RTC_H
