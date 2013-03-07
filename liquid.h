/*
 * liquid.h
 * ----------------------------------------------------------------------------
 * This is explanatory and educational version of controlling a 16x2 
 * Alphanumeric LCD using ATmega 8 bit Microcontrollers. 
 * Copyright (C) 2013 Pasi Heinonen
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
 * Title:	Liquid, LCD driver for HD44780 interface. (liquid.h, hd44780lq.h
 *                                                               and liquid.c)
 * Created:	7.3.2013 23:35
 * Author:	Pasi Heinonen <pasi.heinonen@gmail.com>, https://twitter.com/pasihe
 *
 * Example of controlling a 16x2 Alphanumeric LCD, using an AVR Microcontroller. 
 * Most LCDs have an inbuilt controller which are Hitachi HD44780 compatible. 
 * HD44780 is Hitachi's LCD interface which has become industry standard.
 * It could takes while to understand what is needed to program and interact 
 * with LCD. LCDs has builtin microcontroller and programming it is a 
 * communication between your MCU and LCD MCU.
 *
 * HD44780 Datasheet: (http://lcd-linux.sourceforge.net/pdfdocs/hd44780.pdf) 
 *
 *
 * usage:
 *
	#include "liquid.h"
	
	// set ATmega command pins, if not set the default 5,6,7 used 
	#define PIN_LCD_E 5
	#define PIN_LCD_RW 6
	#define PIN_LCD_RS 7
	
	// set ATmega command and data ports and directions, defaults below
	#define MCU_COMMAND_DDR DDRC
	#define MCU_COMMAND_PORT PORTC
	#define MCU_DATA_DDR DDRB
	#define MCU_DATA_PORT PORTD
	
	int main()
	{
		// initialize ports and reset LCD
		lq_port_configuration();
		lq_init();
		lq_write_string("Hello World");
		_delay_ms(2000);
		lq_write_16bit_number(2^16-1);
		
		while(1)
		{
		}			
		
	}		
 */


#ifndef LIQUID_H_
#define LIQUID_H_

#include <avr/io.h>
#include <util/delay.h>
#include "hd44780lq.h"

typedef unsigned char BYTE;

void lq_write_instruction(BYTE instruction);
void lq_write_data(BYTE data);
void lq_write_string(BYTE* data);
void lq_write_16bit_number(long number);
void lq_waitbusy();
void lq_clear_display();
void lq_port_configuration();
void lq_init();
BYTE lq_read_instruction();

#endif /* LIQUID_H_ */