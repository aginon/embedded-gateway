/*
 * agn_serial.h
 *
 *  Created on: Nov 26, 2017
 *      Author: Kris
 */

#ifndef AGN_SERIAL_H_
#define AGN_SERIAL_H_

#include "config.h"
#include "agn_packet.h"

class AgnSerial {
public:
	void send(struct AGN_PACKET* packet) {
		uint8_t bytes[AGN_PACKET_SIZE];
		AGN_PACKET_SERIALIZE(packet, bytes);
		MASTER_SERIAL.write(bytes, AGN_PACKET_SIZE);
	}

	void receive(struct AGN_PACKET* packet) {
		uint8_t bytes[AGN_PACKET_SIZE];
		MASTER_SERIAL.readBytes(bytes, AGN_PACKET_SIZE);
		AGN_PACKET_DESERIALIZE(packet, bytes);
	}
};



#endif /* AGN_SERIAL_H_ */
