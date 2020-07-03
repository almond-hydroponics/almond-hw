#ifndef PLATFORM_H
#define PLATFORM_H


class Platform_ESP8266
{
public:
	static void setup();
	void loop();

	bool connected() const;    // returns true if wlan has been acquired.
	static unsigned int get_free_heap();

protected:
	bool wifi_ok;

	static void setup_wifi();
	static void setup_i2c();
	static void setup_ota();
	static void setup_fs();

	void loop_wifi();
	static void loop_ota();
};

extern Platform_ESP8266 PLATFORM;


#endif //PLATFORM_H
