/*
 * liquid.c
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
  *                                                              and liquid.c)
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
 */

#include "liquid.h"
#include <stdlib.h>


/************************************************************************/
/* Write command to LCD                                                 */
/* commands described in datasheet Table 6                              */
/************************************************************************/
void lq_write_instruction(BYTE instruction)
{
	LCD_SET_INSTRUCTION_MODE;
	MCU_SET_DATA_OUT;
	LCD_SET_WRITE_MODE;
	
	/* we must enable LCD every time before command write */
	LCD_SET_CLOCK_ENABLED_HIGH;
	
	/* give some time to slow LCD to read the databus */
	_delay_us(15);
	
	MCU_DATA_PORT = instruction;
	
	/* Disable LCD again by setting no voltage to command signals */
	LCD_SET_CLOCK_ENABLED_LOW;
	
	/* Allow some more delay */
	_delay_us(5);
	
	lq_waitbusy();
}

/************************************************************************/
/* Write character to LCD                                               */
/************************************************************************/
void lq_write_data(BYTE data)
{
	LCD_SET_DATA_MODE;
	MCU_SET_DATA_OUT;
	LCD_SET_WRITE_MODE;
	
	/* we must enable LCD every time before command write */
	LCD_SET_CLOCK_ENABLED_HIGH;
	
	/* give some time to slow LCD to read the databus */
	_delay_us(15);
	
	MCU_DATA_PORT = data;
	
	/* Disable LCD again by setting no voltage to command signals */
	LCD_SET_CLOCK_ENABLED_LOW;
	
	/* Allow some more delay */
	_delay_us(5);
	
	lq_waitbusy();	
}

/************************************************************************/
/* Write string to LCD                                                  */
/************************************************************************/
void lq_write_string(BYTE* data)
{
	uint16_t i=0;
	/* write char until terminator mark */
	while(data[i] !='\0')
	{
		lq_write_data(data[i]);
		i++;
	}
}	

/************************************************************************/
/* Write 16 bit number to LCD											*/
/************************************************************************/
void lq_write_16bit_number(long number)
{
	char num[16];
	itoa(number,num,10);
	lq_write_string(num);
}

/************************************************************************/
/* Wait until lcd operations are done (lcd resets BF to 0x00            */
/************************************************************************/
void lq_waitbusy()
{
	/* Read busy flag and address reads the busy flag (BF) indicating that the 
	 * system is now internally operating on a previously received instruction. 
	 * If BF is 1, the internal operation is in progress. The next instruction
	 * will not be accepted until BF is reset to 0. 
	 */
	BYTE busy_flag = 0x80;
	while((LCD_INSTRUCTION_BUSY_FLAG & busy_flag)==LCD_INSTRUCTION_BUSY_FLAG)
	{
		busy_flag = lq_read_instruction();
	}
}

/************************************************************************/
/* Clears the display                                                   */
/************************************************************************/
void lq_clear_display()
{
	lq_write_instruction(LCD_INSTRUCTION_CLEAR_DISPLAY);
}

/************************************************************************/
/* Initializes MCU ports using with LCD                                 */
/************************************************************************/
void lq_port_configuration()
{
	_delay_ms(20);
	MCU_SET_DATA_OUT;
	LCD_INIT_PORTS;
	LCD_SET_INSTRUCTION_MODE;
	LCD_SET_CLOCK_ENABLED_LOW;
	_delay_ms(5);
}

/************************************************************************/
/* Initializes the LCD                                                  */
/* See more from datasheet "Initializing by Internal Reset Circuit"     */
/************************************************************************/
void lq_init()
{
	/* 1. Display clear
	   2. Function set:
	      DL = 1; 8-bit interface data
	      N = 0; 1-line display
	      F = 0; 5 ´ 8 dot character font
	   3. Display on/off control:
	      D = 0; Display off
	      C = 0; Cursor off
	      B = 0; Blinking off
	   4. Entry mode set:
	      I/D = 1; Increment by 1
	      S = 0; No shift
	*/
	_delay_ms(200);
	for(int repeat=0;repeat<3;repeat++)
	{
		lq_write_instruction(LCD_INSTRUCTION_RETURN_HOME);
		_delay_ms(10);
	}
	
	_delay_ms(40);
	lq_write_instruction(LCD_INSTRUCTION_CLEAR_DISPLAY);
	
	lq_write_instruction(LCD_INSTRUCTION_FUNCTION_SET | 
						 LCD_INSTRUCTION_FS_DATA_LENGTH_8BIT | 
		                 LCD_INSTRUCTION_FS_ONE_LINE |
						 LCD_INSTRUCTION_FS_FONT_5X10_DOTS);
						 
	lq_write_instruction(LCD_INSTRUCTION_DISPLAY_CONTROL |
						 LCD_INSTRUCTION_DIS_DISPLAY_ON |
						 LCD_INSTRUCTION_DIS_CURSOR_ON |
						 LCD_INSTRUCTION_DIS_CURSOR_NO_BLINK);
	
	lq_write_instruction(LCD_INSTRUCTION_ENTRY_MODE |
					     LCD_INSTRUCTION_ENTRY_INCR |
						 LCD_INSTRUCTION_ENTRY_SHIFT_CURSOR);
						 
	lq_write_instruction(LCD_INSTRUCTION_CLEAR_DISPLAY);
	
	lq_write_instruction(LCD_INSTRUCTION_RETURN_HOME);
}


/************************************************************************/
/* Read data from LCD                                                   */
/************************************************************************/
BYTE lq_read_instruction()
{
	LCD_SET_INSTRUCTION_MODE;
	MCU_SET_DATA_IN;
	LCD_SET_READ_MODE;
	LCD_SET_CLOCK_ENABLED_HIGH;
	_delay_us(15);
	char ret = MCU_DATA_PORT;
	LCD_SET_CLOCK_ENABLED_LOW;
	_delay_us(15);
	return ret;
}


