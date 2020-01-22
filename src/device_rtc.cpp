#include "device_rtc.h"


#include "logger.h"

Device_rtc::Device_rtc(const char *name)
	:
	Device_input(name)
{
}

void Device_rtc::setup()
{
	LOG_INFO("RTC boot!");

	if (!rtc.begin()) {
		LOG_ERROR("Couldn't find RTC");
		while (1);
	}

	if (rtc.lostPower()) {
		LOG_INFO("RTC lost power, lets set the time!");

		// following line sets the RTC to the date & time this sketch was compiled
		RTC_DS3231::adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
	else {
		LOG_ERROR("RTC is not running!");
	}
}

void Device_rtc::time_of_day(Config_run_table_time *time)
{
	DateTime now(value);

	time->hour = (now.hour() + CONFIG.timezone_h) % 24;
	time->minute = now.minute();
	time->second = now.second();
}

void Device_rtc::update_time(uint32_t ntp_time)
{
	RTC_DS3231::adjust(DateTime(ntp_time));
}

void Device_rtc::loop()
{
	if (!timer.check(1000))
		return;

	timer.reset();
	DateTime now = RTC_DS3231::now();
	value =
		now.unixtime(); // this loses one bit,   math.log( 365*24*60*60*60 )/math.log(2) = 30.81738694131409 so we should be ok, until = 2030, thats ok.
}
