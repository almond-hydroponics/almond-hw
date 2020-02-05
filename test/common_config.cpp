#include "config.h"

// Note: G++ seems to require that these are in the proper order (same as declared).
const Config CONFIG = {
   .email = {
      .server_host = "smtp.gmail.com",
      .server_port = 465,
      .login = "test@gmail.com",
      .password = "test_password",
      .receiver = "test_email@any.com",
   },
   .wlan = {
     .ssid  = "WLAN_SSID",
     .password = "WLAN_PASSWORD",
   },
   .runtable = {  // Run the pump for 1min every 15min from 7:00 to 20:00
      .active_start = { .hour = 7, .minute = 0, .second = 0 },
      .active_stop = { .hour = 20, .minute = 0, .second = 0 },
      .period_on = { .hour = 0, .minute = 1, .second = 0 },
      .period_off = { .hour = 0, .minute = 14, .second = 0 }
   },
   .pump = {
      .low_level_height_mm  = 400,
      .threshold_water_up_s = 10,
   },
   .mqtt = {
	  .mqtt_server = "134.209.190.108",
	  .mqtt_port = 1883,
	  .mqtt_user = "test",
	  .mqtt_password = "testmqtt"
	},
   .hostname = "waterpump",
   .password = "waterize",
   .timezone_h = 3,
};
