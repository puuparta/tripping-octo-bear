/*
 * hd44780lq.h
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
 */


#ifndef HD44780LQ_H_
#define HD44780LQ_H_


/* From HD44780U datasheet page 24: Because the busy flag is set to 1 while 
 * an instruction is being executed, check it to make sure it is 0 before
 * sending another instruction from the MPU.
 *
 * LCD Pin Functions for HD44780U "standard", datasheet Table 6 Instructions:
 */
#define LCD_INSTRUCTION_RESET 0b00000000
#define LCD_INSTRUCTION_CLEAR_DISPLAY 0b00000001
#define LCD_INSTRUCTION_RETURN_HOME 0b00000010

/* Entry mode */
#define LCD_INSTRUCTION_ENTRY_MODE 0b00000100
#define LCD_INSTRUCTION_ENTRY_INCR 0b00000010
#define LCD_INSTRUCTION_ENTRY_DECR 0b00000000
#define LCD_INSTRUCTION_ENTRY_SHIFT_CURSOR 0b00000001
#define LCD_INSTRUCTION_ENTRY_NOSHIFT_CURSOR 0b00000000

/* Display on/off control bits */
#define LCD_INSTRUCTION_DISPLAY_CONTROL 0b00001000
#define LCD_INSTRUCTION_DIS_DISPLAY_ON 0b00000100
#define LCD_INSTRUCTION_DIS_DISPLAY_OFF 0b00000000
#define LCD_INSTRUCTION_DIS_CURSOR_ON 0b00000010
#define LCD_INSTRUCTION_DIS_CURSOR_OFF 0b00000000
#define LCD_INSTRUCTION_DIS_CURSOR_BLINK 0b00000001
#define LCD_INSTRUCTION_DIS_CURSOR_NO_BLINK 0b00000000

/* Function set bits. Bit pattern: 0 0 1 DL N F — — 
 * Sets interface data length (DL), number of display lines (N), and character font (F).
 * takes 37 ms
 */
#define LCD_INSTRUCTION_FUNCTION_SET 0b00100000
#define LCD_INSTRUCTION_FS_DATA_LENGTH_8BIT 0b00010000
#define LCD_INSTRUCTION_FS_DATA_LENGTH_4BIT 0b00000000
#define LCD_INSTRUCTION_FS_TWO_LINE 0b00001000
#define LCD_INSTRUCTION_FS_ONE_LINE 0b00000000
#define LCD_INSTRUCTION_FS_FONT_5X10_DOTS 0b00000100
#define LCD_INSTRUCTION_FS_FONT_5X8_DOTS 0b00000000
#define LCD_INSTRUCTION_BUSY_FLAG 0b10000000

/* MCU command Pin configuration */
#ifndef PIN_LCD_E
/* prevent compiler error by supplying a default */
# warning "PIN_LCD_E not defined for \"hd44780lq.h>\""
#define PIN_LCD_E 5
#endif

#ifndef PIN_LCD_RW
/* prevent compiler error by supplying a default */
# warning "PIN_LCD_RW not defined for \"hd44780lq.h>\""
#define PIN_LCD_RW 6
#endif

#ifndef PIN_LCD_RS
/* prevent compiler error by supplying a default */
# warning "PIN_LCD_RS not defined for \"hd44780lq.h>\""
#define PIN_LCD_RS 7
#endif


/* MCU Port sets */
#ifndef MCU_COMMAND_DDR
/* prevent compiler error by supplying a default */
# warning "MCU_COMMAND_DDR not defined for \"hd44780lq.h>\""
#define MCU_COMMAND_DDR DDRC
#endif

#ifndef MCU_COMMAND_PORT
/* prevent compiler error by supplying a default */
# warning "MCU_COMMAND_PORT not defined for \"hd44780lq.h>\""
#define MCU_COMMAND_PORT PORTC
#endif

#ifndef MCU_DATA_DDR
/* prevent compiler error by supplying a default */
# warning "MCU_DATA_DDR not defined for \"hd44780lq.h>\""
#define MCU_DATA_DDR DDRB
#endif

#ifndef MCU_DATA_PORT
/* prevent compiler error by supplying a default */
# warning "MCU_DATA_PORT not defined for \"hd44780lq.h>\""
#define MCU_DATA_PORT PORTD
#endif

#define MCU_SET_DATA_IN					MCU_DATA_DDR=0x00
#define MCU_SET_DATA_OUT				MCU_DATA_DDR=0xFF
#define LCD_SET_READ_MODE				MCU_COMMAND_PORT |=(1<<PIN_LCD_RW)	/* set RW bit */
#define LCD_SET_WRITE_MODE				MCU_COMMAND_PORT &=~(1<<PIN_LCD_RW) /* reset RW bit */
#define LCD_SET_DATA_MODE				MCU_COMMAND_PORT |=(1<<PIN_LCD_RS)	/* set data mode */
#define LCD_SET_INSTRUCTION_MODE		MCU_COMMAND_PORT &=~(1<<PIN_LCD_RS)	/* set instruction mode */

	/* LCD Pin Functions in HD44780U "standard"
	 * See more: HD44780U datasheet, page 8.
	 * -------------------------------------------------------------
	 * E
	 *     Enable bit which starts data read/write.
	 *
	 * R/W 
	 *     Selects read or write.
	 *
	 * RS
	 *     Signal which selects registers.
	 *		0: Instruction register (for write) Busy flag:
	 *		   address counter (for read)
	 *		1: Data register (when writing characters to LCD)
	 *
	 * DB4 to DB7 
	 *      Four high order bidirectional tristate data bus pins. 
	 *      Used for data transfer and receive between the MPU and the HD44780U. 
	 *      DB7 can be used as a busy flag.
	 *
	 * DB0 to DB3 
	 *      Four low order bidirectional tristate data bus pins.
	 *		Used for data transfer and receive between the MPU and the HD44780U.
	 *      These pins are not used during 4-bit operation.
	 *
	 * More detailed Instructions can be found in datasheet 
	 * Table 6 Instructions at page 26-28 and Figure 23 8-Bit Interface at page 45
	 */

/* The enable pin is used to latch the data on the data pins. 
 * Signal required to switch high to low to latch the data. The LCD interprets and executes command when enable 
 * signal is brought low. */
#define LCD_SET_CLOCK_ENABLED_HIGH		MCU_COMMAND_PORT |=(1<<PIN_LCD_E)   /* Starts data read/write */
#define LCD_SET_CLOCK_ENABLED_LOW		MCU_COMMAND_PORT &=~(1<<PIN_LCD_E) 


/* Command port direction initialization */
#define LCD_INIT_PORTS MCU_COMMAND_DDR |=(1<<PIN_LCD_RS) | (1<<PIN_LCD_RW) | (1<<PIN_LCD_E)



#endif /* HD44780LQ_H_ */