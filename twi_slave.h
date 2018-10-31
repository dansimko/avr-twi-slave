/* twi_slave.h
 * copyright (c) Daniel Simko
 */

#ifndef __TWI_SLAVE_H__
#define __TWI_SLAVE_H__

#include <stdint.h>

#define TWI_REG_SIZE 64
extern uint8_t twidata[TWI_REG_SIZE];

void twi_init(uint8_t addr);
void twi_stop();
void twi_set_callback(void (*recv_func)(), void (*req_func)());

#if (TWI_REG_SIZE > 254)
#error Maximum size of buffer is 254 bytes
#elif (TWI_REG_SIZE < 2)
#error Minimum size of buffer is 2 bytes
#endif

#endif
