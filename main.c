/* main.c
 * copyright (c) Daniel Simko
 */

#include <avr/io.h>
#include <stdint.h>

#include "twi_slave.h"

#define TWI_ADDRESS 0x28

void data_received() {
	/* do something after receiving data */
}

void data_request() {
	/* do something after data being requested */
}

int main() {
	twi_init(TWI_ADDRESS);
	twi_set_callback(&data_received, &data_request);

	while(1);
	twi_stop();
	return 0;
}
