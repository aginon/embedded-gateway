/*
 * config.h
 *
 *  Created on: Nov 26, 2017
 *      Author: Kris
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Master Serial
 *  (+) Use to communicate with Master Component (STM32F4)
 *  (+) Uses two-way UART communication (available only on Serial0)
 */
#define MASTER_SERIAL Serial
#define MASTER_SERIAL_BAUD_RATE 115200
#define MASTER_SERIAL_CONFIG SERIAL_8N1

/* Logger Serial
 * 	(+) Use to log output (with PIN_D4 as TX)
 *  (+) Uses one-way UART communication (available as Serial1)
 */
#define LOGGER_SERIAL Serial1
#define LOGGER_SERIAL_BAUD_RATE 115200
#define LOGGER_SERIAL_CONFIG SERIAL_8O1

/* WiFi Settings */

#define AGN_GATEWAY_WIFI_SSID "K2"
#define AGN_GATEWAY_WIFI_PASS "ExxonUnicornPassport5200Z"

/* MQTTS Settings */

#define AGN_MQTT_SERVER "test.mosquitto.org"
#define AGN_MQTT_SERVER_PORT 8883
#define AGN_MQTT_FINGERPRINT "7E 36 22 01 F9 7E 99 2F C5 DB 3D BE AC 48 67 5B 5D 47 94 D2"
//String fingerprint = "A2 79 92 D3 42 0C 89 F2 93 D3 51 37 8B A5 F5 67 5F 74 FE 3C";

#endif /* CONFIG_H_ */
