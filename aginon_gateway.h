// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _aginon_gateway_H_
#define _aginon_gateway_H_
#include "Arduino.h"
//add your includes for the project aginon_gateway here
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

//end of add your includes here


//add your function definitions for the project aginon_gateway here

void setup_wifi();
void onReceived(char* topic, byte* payload, unsigned int length);
void reconnect();


//Do not add code below this line
#endif /* _aginon_gateway_H_ */
