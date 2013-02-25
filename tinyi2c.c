/*
 * tinyi2c.c
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
 *
 */

#include "tinyi2c.h"

/************************************************************************/
/* initializes I2C bus interface			                            */
/************************************************************************/
void tinyi2c_init()
{
	DDRD = DDRD & 0xfc; // scl & sda ports to input.
	
	
	/* PD1 SDA (TWI Serial DAta)
	 * When the TWEN bit in TWCR is set (one) to enable the 2-wire Serial Interface, 
	 * pin PD1 is disconnected from the port and becomes the Serial Data I/O pin for
	 * the 2-wire Serial Interface. In this mode, there is a spike filter on the pin 
	 * to suppress spikes shorter than 50 ns on the input signal, and the pin is driven 
	 * by an open drain driver with slew-rate limitation.
	 * See more: datasheet ATmega16/32U4 (page: 77-78, Alternate Functions of Port D). 
	 */
	PORTD |= (1<<PD1); // pull-up bit 1, SDA
	
	/* PD0 SCL (TWI Serial CLock)
	 * When the TWEN bit in TWCR is set (one) to enable the 2-wire Serial Interface, 
	 * pin PD0 is disconnected from the port and becomes the Serial Clock I/O pin for 
	 * the 2-wire Serial Interface. In this mode, there is a spike filter on the pin 
	 * to suppress spikes shorter than 50 ns on the input signal, and the pin is driven 
	 * by an open drain driver with slew-rate limitation.
	 * See more: datasheet ATmega16/32U4 (page: 77-78, Alternate Functions of Port D).
	 */
	PORTD |= (1<<PD0); // pull-up bit 0, SCL
	
	/* datasheet ATmega16/32U4 (page: 231-232, TWI Control Register) 
	 * TWCR=1; // SREG ja sei(), for interrupt use
	 */
	
	/* datasheet ATmega16/32U4 (page: 232-233, TWI Status Register)
	 * TWSR = 0x00 sets prescaler bit TWPS to factor 1, so no prescaler in use.
	 */
	TWSR = 0x00;
	
	/* TWBR selects the division factor for the bit rate generator. The bit rate 
	 * generator is a frequency divider which generates the SCL clock frequency 
	 * in the Master modes. Must be > 10 for stable operation.
	 * See more: datasheet ATmega16/32U4 (page: 231, TWI Bit Rate Register)
	 * and datasheet section "20.5.2 Bit Rate Generator Unit"
	 */
	TWBR = 0x04; // clockfreq = 2mhz/(16+2*4*4^1) = ~41,5khz
	
	
	
	/*
	1. alusta
	2. lämpötilan luku:
		- lähetä start
		- odota, että valmis
		- lähetä osoite + lukubitti
		- odota
		- aktivoi vastaanotin (TWIN, TWEN, TWEA)
		- odota
		- lue data
		- lähetä stop
	*/
}

/************************************************************************/
/* Send start condition, address and data direction.                     */
/************************************************************************/
unsigned char tinyi2c_start(unsigned char sla)
{
	/* Send start condition to TWI Control Register 
	 * The TWCR is used to control the operation of the TWI. It is used to enable 
	 * the TWI, to initiate a Master access by applying a START condition 
	 * to the bus, to generate a Receiver acknowledge, to generate a stop condition, 
	 * and to control halting of the bus while the data to be written to the bus 
	 * are written to the TWDR. It also indicates a write collision if data is 
	 * attempted written to TWDR while the register is inaccessible.
	 * See more: datasheet ATmega16/32U4 (page: 231-232, TWI Control Register).
	 */
	TWCR = (1<<TWINT);		/* TWI initialize and interrupt bit */
	TWCR |=(1<<TWSTA);      /* TWI START Condition Bit */
	TWCR |=(1<<TWEN);       /* TWI Enable Bit */

	/* Wait until TWINT Flag set. This indicates that the START
	 * condition has been transmitted- After a START condition has been 
	 * transmitted, the TWINT Flag is set by hardware, and the status 
	 * code in TWSR will be 0x08. See more at datasheet page 240, 
	 * Table 20-3. Status codes for Master Transmitter Mode and
	 * Table 20-4. Status codes for Master Receiver Mode.
	 */
	while (!(TWCR & (1<<TWINT)));
	
	/* Check value of TWI Status Register. Mask prescaler bits to zero.
	 * We should not compare prescaler bits. This makes status checking 
	 * independent of prescaler setting.
	 * We compare hardware zero masked TWSR: xxxx x000 & 1111 1000 and
	 * check whether result differs from TW_START.
	 * If ok the result is: 0000 1000 & 1111 1000 equals 0000 1000
	 * See more at datasheet page 240,
	 * Table 20-3. Status codes for Master Transmitter Mode and
	 * Table 20-4. Status codes for Master Receiver Mode. 
	 */ 
	if ((TWSR & 0xF8) != TW_START)
		return STATUS_ERROR; //error
	
	/* send address of I2C device in bus and clear TWINT bit in
	 * TWCR to start transmission of address+r/w bit 
	 */
	TWDR = sla;
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	/* Wait for TWINT Flag set. This indicates that the address+read/write
	 * bit has been transmitted, and ACK/NACK has been received. 
	 */
	while(!(TWCR & (1<<TWINT)));
	
	/* Check value of TWI Status Register. Mask prescaler bits to zero.
	 * If SLA+W is transmitted, MT mode is entered, if SLA+R is transmitted,
	 * MR mode is entered. All the status codes mentioned in this section 
	 * assume that the prescaler bits are zero or are masked to zero.
	 */
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		return DEVICE_NOT_FOUND; //error

	return 0;
}

/************************************************************************/
/* Read a byte from the I2C device and wait for more                    */
/************************************************************************/
unsigned char tinyi2c_readbyte_ack()
{
	/* Clear TWINT bit in TWCR to start transmission of data.
	 * TWEN must be written to one to enable the TWI. The TWEA bit must 
	 * be written to one to enable the acknowledgement of the device’s 
	 * own slave address or the general call address. 
	 * TWSTA and TWSTO must be written to zero.
	 * See more at datasheet:
	 * Table 20-4. Status codes for Master Receiver Mode
	 */
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); /* activate receiving */
	
	/* Wait for TWINT Flag set. This indicates that the ACK
	 * condition has been transmitted. 
	 * Data byte will be received and ACK will be returned 
	 */
	while (!(TWCR & (1<<TWINT)));
	
	/* return the received byte */
	return TWDR;
}

/************************************************************************/
/* Read a byte from the I2C device and stop				                */
/************************************************************************/
unsigned char tinyi2c_readbyte_not_ack()
{
	/* Clear TWINT bit in TWCR to start transmission of data.
	 * TWEN must be written to one to enable the TWI. The TWEA bit must 
	 * be written to one to enable the acknowledgement of the device’s 
	 * own slave address or the general call address. 
	 * TWSTA and TWSTO must be written to zero.
	 * See more at datasheet:
	 * Table 20-4. Status codes for Master Receiver Mode
	 */
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	/* Wait for TWINT Flag set. Data byte will be received and NOT ACK 
	 * will be returned. 
	 */
	while (!(TWCR & (1<<TWINT)));
	
	/* return the received byte */
	return TWDR;
}

/************************************************************************/
/* Write byte to I2C				                                    */
/************************************************************************/
unsigned char tinyi2c__write( unsigned char data )
{
	/* Load DATA into TWDR Register. Clear TWINT bit in TWCR to start 
	 * transmission of data.
	 */
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	/* Wait for TWINT Flag set. This indicates that the DATA has been
	 * transmitted, and ACK/NACK has been received.
	 */
	while(!(TWCR & (1<<TWINT)));

	/* Check value of TWI Status Register. Mask prescaler bits to zero.
	 * If SLA+W is transmitted, MT mode is entered, if SLA+R is transmitted,
	 * MR mode is entered.
	 */
	if( TW_STATUS & 0xF8 != TW_MT_DATA_ACK) 
		return STATUS_ERROR;
	
	return 0;
}	

/************************************************************************/
/* Stops the data transfer and releases I2C bus                         */
/************************************************************************/
void tinyi2c_stop()
{
	/* Clear TWINT bit in TWCR to start transmission of data.
	 * STOP condition will be transmitted and TWSTO Flag will be reset.
	 */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	/* wait for stop condition (TWSTO) is executed and bus released 
	 */
	while(TWCR & (1<<TWSTO));
}