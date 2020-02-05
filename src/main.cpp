#include <ESP8266WiFi.h>

#include "config.h"

#include "serial.h"
#include "send_email.h"
#include "logger.h"
#include "webserver.h"
#include "ntp_client.h"
#include <EEPROM.h>

#include "platform.h"
#include "device_rtc.h"

#include "water_level.h"
#include "device_pin_in.h"
#include "device_pin_out.h"
#include "application_logic.h"
#include "push_data.h"

Device_rtc		DEV_RTC("rtc");	// NOLINT(cert-err58-cpp)
Water_level 	DEV_WLEVEL("water_level", PIN_TRIGGER, PIN_ECHO);	// NOLINT(cert-err58-cpp)
Device_pin_in 	DEV_WDETECT("water_detect", PIN_WDETECT, 4, true); // water detect switch - when its physically low, its open -> high	// NOLINT(cert-err58-cpp)
Device_pin_out 	DEV_PUMP("pump", PIN_PUMP, true); // pump inverted, since npn transistor - writing 0 will start the pump	// NOLINT(cert-err58-cpp)
Device_pin_in 	DEV_SWITCH("switch", PIN_SWITCH, 8, true); // manual switch	// NOLINT(cert-err58-cpp)
Logic 			LOGIC;	// NOLINT(cert-err58-cpp)
Push_data		PUSH;	// NOLINT(cert-err58-cpp)

// GLOBAL VARIABLE
String relayPrefix = "RELAY";	// NOLINT(cert-err58-cpp)
String sensorPrefix = "SENSOR";	// NOLINT(cert-err58-cpp)

// DATA FOR THIS DEVICE ONLY
int sensorPin5 = 5;
bool sensorPin5enabled = false;
int lastSensorPin5Value = 1;
int sensorPin6 = 6;
bool sensorPin6enabled = false;
int lastSensorPin6Value = 1;
int sensorPin7 = 7;
bool sensorPin7enabled = false;
int lastSensorPin7Value = 1;
int sensorPin8 = 8;
bool sensorPin8enabled = false;
int lastSensorPin8Value = 1;

long previousMillis = 0; 
unsigned long postInterval = 10000;

int pinConverter(int boardPin) {
    if (boardPin == 0) return 16;
    if (boardPin == 1) return 5;
    if (boardPin == 2) return 4;
    if (boardPin == 3) return 0;
    if (boardPin == 4) return 2;
    if (boardPin == 5) return 14;
    if (boardPin == 6) return 12;
    if (boardPin == 7) return 13;
    if (boardPin == 8) return 15;
}

void setupPinModes()
{
	pinMode(pinConverter(0), OUTPUT);
	pinMode(pinConverter(1), OUTPUT);
	pinMode(pinConverter(2), OUTPUT);
	pinMode(pinConverter(3), OUTPUT);
	pinMode(pinConverter(4), OUTPUT);

	pinMode(pinConverter(5), INPUT_PULLDOWN_16);
	pinMode(pinConverter(6), INPUT_PULLDOWN_16);
	pinMode(pinConverter(7), INPUT_PULLDOWN_16);
	pinMode(pinConverter(8), INPUT_PULLDOWN_16);
}

/** Make some artificial devices for more information */
class Device_uptime: public Device_input
{
public:
	Device_uptime()
		: Device_input("uptime")
	{};
	void loop() override
	{ this->value = millis() / 1000; };
};

class Device_status: public Device_input
{
public:
	Device_status()
		: Device_input("status")
	{};
	void loop() override
	{ this->value = (int)LOG.get_status(); };
};

Device_uptime DEV_UPTIME;	// NOLINT(cert-err58-cpp)

Device_status DEV_STATUS;// NOLINT(cert-err58-cpp)

Device *const DEVICES[] =
	{&DEV_WLEVEL, &DEV_WDETECT, &DEV_PUMP, &DEV_UPTIME, &DEV_RTC, &DEV_STATUS,
	 &DEV_SWITCH};

#define DEVICES_N (sizeof(DEVICES)/sizeof(Device*))

static void logger_fatal_hook(const char *log_line)
{
	// if we are not connected, we are not storing the messages for now.
	if (!PLATFORM.connected())
		return;

	int buffer_len = Logger::max_line_len + 128;
	int subject_len = 256;
	char *buffer = (char *)malloc(buffer_len);
	char *subject = (char *)malloc(subject_len);

	memset(buffer, 0x00, buffer_len);
	memset(subject, 0x00, subject_len);

	// out of memory, lets skip the whole thing.
	if (buffer == nullptr || subject == nullptr)
		return;

	snprintf(buffer,
			 buffer_len - 1,
			 "Error on %s: %s.\n",
			 CONFIG.hostname,
			 log_line);
	snprintf(subject,
			 subject_len - 1,
			 "[ESP] %s : error detected",
			 CONFIG.hostname);

	email_send(&CONFIG.email, CONFIG.email.receiver, subject, buffer);

	free(subject);
	free(buffer);
}

static int generate_device_json(char *buffer)
{
	strcpy(buffer, "{\"dev\":[");
	int buffer_offset = strlen(buffer);
	unsigned int loop;
	for (loop = 0; loop < DEVICES_N; loop++) {
		Device *dev = DEVICES[loop];
		int added = dev->jsonify(buffer + buffer_offset,
								 WEBSERVER_MAX_RESPONSE_SIZE - buffer_offset);

		if (added == 0)
			break;

		buffer_offset += added;

		if (buffer_offset + 2 >= WEBSERVER_MAX_RESPONSE_SIZE)
			break;

		buffer[buffer_offset] = ',';
		buffer[buffer_offset + 1] = 0;
		buffer_offset += 1;
	}

	if (loop < DEVICES_N) // exited with break
	{
		return 0;
	}
	else {
		buffer[buffer_offset - 1] = ']';
		buffer[buffer_offset] = '}';
		buffer[buffer_offset + 1] = 0;
		return (buffer_offset + 1);
	}
}

static void handle_get_devices()
{
	char *buffer = webserver_get_buffer();

	if (buffer == nullptr)
		return;

	int blen = generate_device_json(buffer);

	if (blen == 0)
		WEBSERVER
			.send(500, "application/json", R"({"error":"out of buffer"})");
	else
		WEBSERVER.send(200, "application/json", buffer);

	free(buffer);
}

static bool handle_push_devices(bool force)
{
	int values[6];
	for (unsigned int loop = 0; loop < 6; loop++)
		values[loop] = DEVICES[loop]->get_value();
	return PUSH.thingspeak_push((const int *)values, 0, 6, force);
}

static bool handle_set_email()
{
	LOG_INFO("Status email requested.");
	const int subject_len = 256;
	char *subject = (char *)malloc(subject_len);
	char *buffer = (char *)malloc(WEBSERVER_MAX_RESPONSE_SIZE);
	memset(subject, 0x00, subject_len);
	memset(buffer, 0x00, WEBSERVER_MAX_RESPONSE_SIZE);

	snprintf(subject,
			 subject_len,
			 "[ESP] %s : Status report",
			 CONFIG.hostname);
	int blen = generate_device_json(buffer);
	serial_print_raw(buffer, blen, true);
	bool ret =
		email_send(&CONFIG.email, CONFIG.email.receiver, subject, buffer);

	free(subject);
	free(buffer);
	return ret;
}

static bool handle_set_ntp()
{
	uint32_t ntp_time = ntp_update();
	if (ntp_time == 0) {
		return false;
	}
	Device_rtc::update_time(ntp_time);
	return true;
}

static void handle_http(bool ret)
{
	char *buffer = webserver_get_buffer();
	if (buffer == nullptr)
		return;

	int resp_code = ret ? 200 : 500;
	const char *code = ret ? "ok" : "err";

	snprintf(buffer, WEBSERVER_MAX_RESPONSE_SIZE, R"({"status":"%s"})", code);
	WEBSERVER.send(resp_code, "application/json", buffer);
	free(buffer);
}

void handle_get_time()
{
	Config_run_table_time time{};
	char *buffer = webserver_get_buffer();
	if (buffer == nullptr)
		return;

	DEV_RTC.time_of_day(&time);

	snprintf(buffer,
			 WEBSERVER_MAX_RESPONSE_SIZE,
			 R"({"hour":%d,"min":%d,"sec":%d})",
			 time.hour,
			 time.minute,
			 time.second);
	WEBSERVER.send(200, "application/json", buffer);
	free(buffer);
}

void add_password_protected(const char *url, void (*handler)())
{
	char *buffer = (char *)malloc(1024);
	strcpy(buffer, "/set/");
	strcat(buffer, CONFIG.password);
	strcat(buffer, "/");
	strcat(buffer, url);
	WEBSERVER.on(buffer, handler);
	free(buffer);
}

void sendHeartBeatOnInterval()
{
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis > postInterval) {
		previousMillis = currentMillis;
		// UPDATE DATA EVERY [postInterval] Seconds
		// Send heartbeat
	}
}

void handle_serial()
{
	int line_len;
	char *line = serial_receive(&line_len);

	if (line == nullptr) return;

	LOG_WARN("Serial: %s ", line);

	if (strcmp(line, "email") == 0) {
		handle_set_email();
	}
	else {
		serial_print("Invalid command\n");
	}
}

void setup()
{
	// SETUP ESP8266 DEVICE
	LOG.setup_serial(CONFIG.hostname, 9600);
	LOG.setup_led(PIN_LED);
	LOG.setup_fatal_hook(logger_fatal_hook);
	Platform_ESP8266::setup();

	webserver_setup();

	for (auto loop : DEVICES)
		loop->setup();

	LOG.set_status(Logger::Status::RUNNING);
	WEBSERVER.on("/get/dev", handle_get_devices);
	WEBSERVER.on("/get/time", handle_get_time);
	add_password_protected("ntp", []
	{ handle_http(handle_set_ntp()); });
	//out of memory: add_password_protected("email", handle_set_email );
}

void loop()
{
	LOG.loop();
	handle_serial();

	static unsigned long avail_memory_last = 0xFFFF;
	unsigned long avail_memory_now = Platform_ESP8266::get_free_heap();
	if (avail_memory_now < avail_memory_last) {
		LOG_INFO("Available memory dropped: %u", avail_memory_now);
		avail_memory_last = avail_memory_now;
	}

	webserver_loop();
	PLATFORM.loop();

//	sendHeartBeatOnInterval();
	delay(10);

	for (auto loop : DEVICES)
		loop->loop();

	Config_run_table_time time_now{};
	DEV_RTC.time_of_day(&time_now);

//	LOG_INFO("The temperature is: %f", RTC_DS3231::getTemperature());
//	sendHeartBeatOnInterval();

	bool logic_changed = LOGIC.run_logic(&time_now,
										 &DEV_PUMP,
										 DEV_WLEVEL.get_value(),
										 DEV_WDETECT.get_value(),
										 DEV_SWITCH.get_value());

	if (logic_changed) {
		LogicStatus status = LOGIC.get_status();

		// if new status is idle, then we were pumping -> push the delays (assuming valid)
		if (status == LogicStatus::idle) {
			int delays[2];
			bool valid = LOGIC.get_measurements(delays);
			if (valid) {
				PUSH.thingspeak_push((const int *)delays, 6, 2, true);
			}
		}
		else if (status == LogicStatus::pump_started
			|| status == LogicStatus::draining) {
			handle_push_devices(true);
		}
	}
	else {
		handle_push_devices(false);
	}
}
