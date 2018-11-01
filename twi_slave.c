/* twi_slave.c
 * copyright (c) 2018 Daniel Simko
 */

#include <avr/interrupt.h>
#include <util/twi.h>

#include "twi_slave.h"

uint8_t twidata[TWI_REG_SIZE];
uint8_t twi_reg_addr = TWI_REG_SIZE;

void (*data_recv_call)() = 0x00;
void (*data_req_call)() = 0x00;

void twi_init(uint8_t addr) {
	/* Clear interrupts */
	cli();

	/* set TWI/I2C address */
	TWAR = (addr << 1);

	/* Enable and clear TWI interrupt, enable address matching */
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);

	/* enable interrupts */
	sei();
}

void twi_stop() {
	/* clear ACK and enable bits */
	TWCR &= ~((1<<TWEA) | (1<<TWEN));
}

void twi_set_callback(void (*recv_call)(), void (*req_call)()) {
	data_recv_call = recv_call;
	data_req_call = req_call;
}

ISR(TWI_vect) {
	switch(TW_STATUS) {
		/*case TW_START:
		case TW_REP_START: */
		case TW_SR_SLA_ACK:
			twi_reg_addr = TWI_REG_SIZE;
			/* clear TWI interrupt flag, ACK for recv next byte */
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			break;
		/*case TW_SR_ARB_LOST_SLA_ACK:
		case TW_SR_GCALL_ACK:
		case TW_SR_ARB_LOST_GCALL_ACK:*/
		case TW_SR_DATA_ACK:
			if (twi_reg_addr >= TWI_REG_SIZE) {
				if (TWDR < TWI_REG_SIZE) {
					twi_reg_addr = TWDR;
				} else {
					twi_reg_addr = 0x00;
				}
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			} else {
				if (twi_reg_addr < TWI_REG_SIZE) {
					twidata[twi_reg_addr] = TWDR;
					twi_reg_addr++;
					TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
				} else {
					TWCR = (1<<TWIE) | (1<<TWINT) | (0<<TWEA) | (1<<TWEN);
				}
			}
			break;
		/*case TW_SR_DATA_NACK:
		case TW_SR_GCALL_DATA_ACK:
		case TW_SR_GCALL_DATA_NACK:*/
		case TW_SR_STOP:
			data_recv_call();
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			break;
		case TW_ST_SLA_ACK:
			data_req_call();
			TWDR = twidata[twi_reg_addr];
			twi_reg_addr++;
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			break;
		/*case TW_ST_ARB_LOST_SLA_ACK:*/
		case TW_ST_DATA_ACK:
			TWDR = twidata[twi_reg_addr];
			twi_reg_addr++;

			if (twi_reg_addr < TWI_REG_SIZE) {
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			} else {
				TWCR = (1<<TWIE) | (1<<TWINT) | (0<<TWEA) | (1<<TWEN);
			}
			break;
		/*case TW_ST_DATA_NACK:
		case TW_ST_LAST_DATA:
		case TW_NO_INFO:*/
		case TW_BUS_ERROR:
			TWCR = 0;
			TWCR = (1<<TWIE) | (1<<TWINT) | (0<<TWEA) | (1<<TWEN);
			break;
		default:
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			break;
	}
}
