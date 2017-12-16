/*
 * agn_packet.h
 *
 *  Created on: Dec 16, 2017
 *      Author: Kris
 */

#ifndef AGN_PACKET_H_
#define AGN_PACKET_H_

#include "stdint.h"

struct AGN_PACKET_SIZES {
	uint8_t magic;
	uint8_t depth;
	uint8_t hex1;
	uint8_t hex2;
};

extern const struct AGN_PACKET_SIZES AGN_PACKET_SIZES_CONFIG;

struct AGN_PACKET {
	uint16_t magic:16;
	uint32_t depth:32;
	uint8_t hex1:4;
	uint8_t hex2:4;
};

// SIZE IN BYTES
extern const int AGN_PACKET_SIZE;

void AGN_PACKET_SERIALIZE(struct AGN_PACKET *packet, uint8_t* bytes);
void AGN_PACKET_DESERIALIZE(struct AGN_PACKET *packet, uint8_t* bytes);

/*
 * ==== SERIALIZE USAGE ====
 * uint8_t packet[AGN_PACKET_SIZE];
 * struct AGN_PACKET packet_struct = {110781};
 * AGN_PACKET_SERIALIZE(&packet_struct, packet);
 *
 * ==== DESERIALIZE USAGE ====
 * uint8_t packet[AGN_PACKET_SIZE] = {0x0, 0x1A, 0x88, 0xC9};
 * struct AGN_PACKRET packet_struct;
 * AGN_PACKET_DESERIALIZE(&packet_struct, packet);
 *
 */


#endif /* AGN_PACKET_H_ */
