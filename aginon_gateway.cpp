// Do not remove the include below
#include "aginon_gateway.h"
#include "agn_packet.h"
#include "agn_serial.h"

// WiFi Variables

static const char* ssid             = AGN_GATEWAY_WIFI_SSID;
static const char* password         = AGN_GATEWAY_WIFI_PASS;

// MQTT Varibles

static const char* mqtt_server      = AGN_MQTT_SERVER;
static const int   mqtt_port        = AGN_MQTT_SERVER_PORT;
static const char* mqtt_fingerprint = AGN_MQTT_FINGERPRINT;

WiFiClientSecure espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
bool isSslVerified = false;

AgnSerial master;

void setup_wifi() {

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

	// Verify MQTT server

	bool isConnected;
	while(!isSslVerified){
		isConnected = espClient.connect(mqtt_server, mqtt_port);
		if (!isConnected) {
			LOGGER_SERIAL.printf("Unable to connect to %s:%d\r\nRetrying in 5 seconds...\r\n", mqtt_server, mqtt_port);
			delay(5000);
			continue;
		}

		isSslVerified = espClient.verify(mqtt_fingerprint, mqtt_server);
		espClient.stop();

		if (isSslVerified) {
			LOGGER_SERIAL.println("MQTT Server fingerprint matches!");
			break;
		} else {
			LOGGER_SERIAL.println("MQTT Server fingerprint doesn't match!\r\nRetrying in 5 seconds...\r\n");
			delay(5000);
			continue;
		}
	}
}

void onReceived(char* topic, byte* payload, unsigned int length) {
	LOGGER_SERIAL.print("Message arrived [");
	LOGGER_SERIAL.print(topic);
	LOGGER_SERIAL.print("] ");
	for (size_t i = 0; i < length; i++) {
		LOGGER_SERIAL.print((char) payload[i]);
	}
	LOGGER_SERIAL.println();

	// Switch on the LED if an 1 was received as first character
	if ((char) payload[0] == '1') {
		digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
		// but actually the LED is on; this is because
		// it is active low on the ESP-01)
	} else {
		digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
	}

}

void reconnect() {
	// Loop until we're reconnected

	while (!client.connected()) {
		LOGGER_SERIAL.printf("Attempting MQTT connection to %s:%d...", mqtt_server, mqtt_port);
		// Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		if (client.connect(clientId.c_str())) {
			LOGGER_SERIAL.println(" connected!");
			// Once connected, publish an announcement...
			client.publish("/qos0", "SLV0717 connected!");
			// ... and resubscribe
			client.subscribe("inTopic");
		} else {
			LOGGER_SERIAL.print("failed, rc=");
			LOGGER_SERIAL.print(client.state());
			LOGGER_SERIAL.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void setup() {
	pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
	MASTER_SERIAL.begin(MASTER_SERIAL_BAUD_RATE, MASTER_SERIAL_CONFIG);
	LOGGER_SERIAL.begin(LOGGER_SERIAL_BAUD_RATE, LOGGER_SERIAL_CONFIG);

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
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(onReceived);
}

void loop() {
	if (isSslVerified) {
		if (!client.connected()) {
			reconnect();
		}
		client.loop();

		long now = millis();
		if (now - lastMsg > 120) {
			// Publish MQTT
			lastMsg = now;
			//++value;
			//strcpy(msg, "SLV0717 Heartbeat");
			//LOGGER_SERIAL.print("Publish message: ");
			//LOGGER_SERIAL.println(msg);
			//client.publish("/qos0", msg);

			// Receive Message
			struct AGN_PACKET packet;
			master.receive(&packet);
			LOGGER_SERIAL.print("Received Packet: (mg = ");
			LOGGER_SERIAL.print(packet.magic, HEX);
			LOGGER_SERIAL.print(", dp = ");
			LOGGER_SERIAL.print(packet.depth);
			LOGGER_SERIAL.print(", hex1 = ");
			LOGGER_SERIAL.print(packet.hex1, HEX);
			LOGGER_SERIAL.print(", hex2 = ");
			LOGGER_SERIAL.print(packet.hex2, HEX);
			LOGGER_SERIAL.println(")");
		}
	} else {
		LOGGER_SERIAL.println("SSL not Verified!");
		delay(1000);
	}
}

