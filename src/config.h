#ifndef ALMOND_HW_CONFIG_H
#define ALMOND_HW_CONFIG_H

#include <cstdint>
#include <type_traits>

template<typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

// User configurations are provided as structs so that it will be easier to
// change and to read.

struct Config_email {
	const char *server_host;
	int server_port;
	const char *login;
	const char *password;
	const char *receiver;
};

struct Config_wlan {
	const char *ssid;
	const char *password;
};

struct Config_run_table_time {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

struct Config_run_table {
	Config_run_table_time active_start;
	Config_run_table_time active_stop;
	Config_run_table_time period_on;
	Config_run_table_time period_off;
};

struct Config_pump {
	uint16_t low_level_height_mm;
	uint8_t threshold_water_up_s;
};

struct Config_mqtt {
	const char *mqtt_server;
	int mqtt_port;
	const char *mqtt_user;
	const char *mqtt_password;
};

struct Config {
	Config_email email;
	Config_wlan wlan;
	Config_run_table runtable;
	Config_pump pump;
	Config_mqtt mqtt;
	const char *hostname;
	const char *password;
	uint8_t timezone_h;
};

extern const Config CONFIG;

// Hardware configurations fixed

#define PIN_RTC_SCL 14
#define PIN_RTC_SDA 12
#define PIN_LED     15
#define PIN_PUMP    4
#define PIN_SWITCH  16
#define PIN_TRIGGER 2
#define PIN_ECHO    5
#define PIN_WDETECT 13

#endif //ALMOND_HW_CONFIG_H
