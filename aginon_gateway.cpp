// Do not remove the include below
#include "aginon_gateway.h"
#include "agn_packet.h"
#include "agn_serial.h"

#include "ESP8266WiFi.h"
#include "../firebase/src/FirebaseArduino.h"

// WiFi Variables

static const char* ssid             = AGN_GATEWAY_WIFI_SSID;
static const char* password         = AGN_GATEWAY_WIFI_PASS;

WiFiClientSecure espClient;

long lastMsg = 0;
char msg[50];
int value = 0;
bool isSslVerified = false;

const int WIFI_UNCONNECTED_PIN = 14;	// D5
const int AGN_HB_IN_PIN = 12;			// D6
const int AGN_HB_OUT_PIN = 13;			// D7

AgnSerial master;
StaticJsonBuffer<512> jsonBuffer;

void setup_wifi() {

	digitalWrite(WIFI_UNCONNECTED_PIN, HIGH);

	delay(10);
	// We start by connecting to a WiFi network
	LOGGER_SERIAL.println();
	LOGGER_SERIAL.printf("Connecting to %s", ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		LOGGER_SERIAL.print(".");
	}

	randomSeed(micros());

	LOGGER_SERIAL.println(" connected!");
	LOGGER_SERIAL.print("IP address: ");
	LOGGER_SERIAL.println(WiFi.localIP());
	LOGGER_SERIAL.print("MAC address: ");
	LOGGER_SERIAL.println(WiFi.macAddress());
	LOGGER_SERIAL.print("Subnet Mask: ");
	LOGGER_SERIAL.println(WiFi.subnetMask());

	digitalWrite(WIFI_UNCONNECTED_PIN, LOW);
}


static uint32_t prevMicros = 0;
uint32_t profile() {
	uint32_t now = micros();
	uint32_t retval = now - prevMicros;
	prevMicros = now;
	return retval;
}

void setup() {
	pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
	MASTER_SERIAL.begin(MASTER_SERIAL_BAUD_RATE, MASTER_SERIAL_CONFIG);
	LOGGER_SERIAL.begin(LOGGER_SERIAL_BAUD_RATE, LOGGER_SERIAL_CONFIG);

	pinMode(WIFI_UNCONNECTED_PIN, OUTPUT);
	digitalWrite(WIFI_UNCONNECTED_PIN, HIGH);

	pinMode(AGN_HB_IN_PIN, INPUT_PULLUP);

	LOGGER_SERIAL.printf("\r================================================================================\r\n");
	LOGGER_SERIAL.printf("                   Logger for Aginon-IoT Gateway (esp8266)\r\n");
	LOGGER_SERIAL.printf("\r\n");
	LOGGER_SERIAL.printf(" If you are seeing this, then you have successfully connected to the debug/log  \r\n");
	LOGGER_SERIAL.printf(" port of the Gateway! Aginon-IoT is a project for 2110363 HW SYN LAB, Department\r\n");
	LOGGER_SERIAL.printf(" of Computer Engineering, Chulalongkorn University. For more information, please\r\n");
	LOGGER_SERIAL.printf(" checkout https://github.com/aginon .\r\n");
	LOGGER_SERIAL.printf("                                                            (C) Aginon-IoT 2017 \r\n");
	LOGGER_SERIAL.printf("\r================================================================================\r\n");
	LOGGER_SERIAL.printf("Starting...\r\n");

	setup_wifi();

	Firebase.begin(AGN_FIREBASE_HOST, AGN_FIREBASE_KEY);
	LOGGER_SERIAL.println("Firebase initialized!");
}

void loop() {
	if (!WiFi.isConnected()) {
		LOGGER_SERIAL.println("WiFi connection lost");
		setup_wifi();
	}
	long now = millis();
	if (now - lastMsg > 50) {
		// Publish MQTT
		lastMsg = now;
		//++value;
		//strcpy(msg, "SLV0717 Heartbeat");
		//LOGGER_SERIAL.print("Publish message: ");
		//LOGGER_SERIAL.println(msg);
		//client.publish("/qos0", msg);

		// Receive Message
		struct AGN_PACKET packet;
		LOGGER_SERIAL.printf("Before Receive: %u", profile());
		LOGGER_SERIAL.println();
		master.receive(&packet);

		LOGGER_SERIAL.printf("After Receive: %u", profile());
		LOGGER_SERIAL.println();

		LOGGER_SERIAL.print("Received Packet: (mg = ");
		LOGGER_SERIAL.print(packet.magic, HEX);
		LOGGER_SERIAL.print(", dp1 = ");
		LOGGER_SERIAL.print(packet.depth1);
		LOGGER_SERIAL.print(", hex1 = ");
		LOGGER_SERIAL.print(packet.hex1, HEX);
		LOGGER_SERIAL.print(", hex2 = ");
		LOGGER_SERIAL.print(packet.hex2, HEX);
		LOGGER_SERIAL.println(")");

		// Firebase get packet

		// Update packet
		packet.hex1 = 0xF;
		packet.hex2 = 0x5;
		packet.magic = 0xA0A0;
		packet.mode = 0x9A;
		packet.status = 0xDDDD;

		// Firebase send packet

		char json[512];
		sprintf(json,
				  "{\"AGN_DEPTH1\":%d,\"AGN_DEPTH2\":%d}", packet.depth1, packet.depth2);

		LOGGER_SERIAL.printf("JSON: %s\r\n", json);
		JsonVariant variant = jsonBuffer.parse(json);

		LOGGER_SERIAL.printf("Before Firebase Send: %u", profile());
		LOGGER_SERIAL.println();
		//Firebase.set("AGN_DEPTH", variant);
		Firebase.set("AGN_DEPTH/01", packet.depth1);
		Firebase.set("AGN_DEPTH/02", packet.depth2);

		LOGGER_SERIAL.printf("After Firebase Send: %u", profile());
		LOGGER_SERIAL.println();

		// Required delay between receive and send
		//delay(20);

		// Send Message


		LOGGER_SERIAL.printf("Before Send: %u", profile());
		LOGGER_SERIAL.println();
		master.send(&packet);


		LOGGER_SERIAL.printf("After Send: %u", profile());
		LOGGER_SERIAL.println();


		LOGGER_SERIAL.print("Sent Packet: (mg = ");
		LOGGER_SERIAL.print(packet.magic, HEX);
		LOGGER_SERIAL.print(", dp1 = ");
		LOGGER_SERIAL.print(packet.depth1);
		LOGGER_SERIAL.print(", hex1 = ");
		LOGGER_SERIAL.print(packet.hex1, HEX);
		LOGGER_SERIAL.print(", hex2 = ");
		LOGGER_SERIAL.print(packet.hex2, HEX);
		LOGGER_SERIAL.println(")");


	}
}

