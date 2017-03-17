#ifndef _CRC16_CUSTOM_
#define _CRC16_CUSTOM_

#include <stdint.h>
/**
 * crc16 - compute the CRC-16 for the data buffer
 * @crc: previous CRC value
 * @buffer: data pointer
 * @len: number of bytes in the buffer
 *
 * Returns the updated CRC value.
 */
uint16_t crc16( uint8_t const *buffer, uint16_t len, uint16_t crc);
uint8_t checksum8(uint8_t *buffer, uint16_t len);

#endif
