/*
 * tinyi2c.h
 * ----------------------------------------------------------------------------
 * This is explanatory and educational version of I2C Bus driver for
 * ATmega 8 bit Microcontrollers. Copyright (C) 2013 Pasi Heinonen
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * ----------------------------------------------------------------------------
 *
 * Title:	Tiny I2C Master Mode interface (tinyi2c.h and tinyi2c.c)
 * Created:	25.2.2013 21:10:54
 * Author:	Pasi Heinonen <pasi.heinonen@gmail.com>, https://twitter.com/pasihe
 *
 * The TWI can operate in one of four major modes. These are named
 * Master Transmitter (MT), Master Receiver (MR), Slave Transmitter (ST)
 * and Slave Receiver (SR). Several of these modes can be used in the same
 * application. As an example, the TWI can use MT mode to write data into a
 * TWI EEPROM, MR mode to read the data back from the EEPROM. If other masters
 * are present in the system, some of these might transmit data to the TWI,
 * and then SR mode would be used. It is the application software that decides
 * which modes are legal.
 *
 * tinyI2C implements Master Transmitter (MT) and Master Receiver (MR).
 * Comments inline refers to particular data sheet sections.
 * Please read further from ATmega16/32U4 data sheet chapter 20.
 * 2-wire Serial Interface.
 *-----------------------------------------------------------------------------
 *
 * usage:
 *
 *	#include "tinyi2c.h"
 *
 *	int main()
 *	{
 *		DDRB = 0xff; // portb for output, for LCD etc.
 *		unsigned char deviceAddress = 0x96;
 *		tinyi2c_init();
 *		tinyi2c_start(deviceAddress & I2CREAD);
 *
 *		while(1)
 *		{
 *			// read byte to portb directly
 *			PORTB = tinyi2c_readbyte_ack();
 *		}
 *
 *		// stop inside ISR etc.
 *		tinyi2c_stop();
 *  }
 *
 */


#ifndef TINYI2C_H
#define TINYI2C_H

#include <avr/io.h>
#include <util/twi.h>

/* start condition for TWI Status Register*/
#define START (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)

/* error status when start condition setting fails */
#define STATUS_ERROR 1

/* error status when device address sending fails */
#define DEVICE_NOT_FOUND 2

/* address of i2c device */
#define DEVICEADDR 0

/* data direction (reading from I2C device) */
#define I2CREAD    1

/* data direction (writing to I2C device) */
#define I2CWRITE   0

#define CLEARBITS(port,bits) port &= ~(bits)

#define OUTPUT_PORT 0

extern void tinyi2c_init();

extern unsigned char tinyi2c_start(unsigned char address);

extern unsigned char tinyi2c_readbyte_ack();

extern unsigned char tinyi2c_readbyte_not_ack();

extern unsigned char tinyi2c__write(unsigned char data);

extern void tinyi2c_stop();

/* not in use */
struct TinyI2C
{
	unsigned char TxPort;
	unsigned char RxPort;
	unsigned char DeviceAddr;
	unsigned char DataDirection;
} tinyi2c;

#endif /* TINYI2C_H */