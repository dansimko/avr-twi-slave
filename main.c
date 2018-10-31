/* main.c
 * copyright (c) Daniel Simko
 */

#include <avr/io.h>
#include <stdint.h>

#include "twi_slave.h"

#define TWI_ADDRESS 0x28

void data_received() {
	/* do something */
}

void data_request() {
	for (int i = 0; i < TWI_REG_SIZE; i++) {
		twidata[i] = i;
	}
}

int main() {
	twi_init(TWI_ADDRESS);
	twi_set_callback(&data_received, &data_request);

	while(1);
	twi_stop();
	return 0;
}
