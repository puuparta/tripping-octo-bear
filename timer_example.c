/*
 * timer_example.c
 * ----------------------------------------------------------------------------
 * This is explanatory and educational version of 16bit Timer 
 * for ATmega 8 bit Microcontrollers. Copyright (C) 2013 Pasi Heinonen
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
 * Title:	16bit Timer with interrupt (timer_example.c)
 * Created:	03.03.2013 22:30
 * Author:	Pasi Heinonen <pasi.heinonen@gmail.com>, https://twitter.com/pasihe
 *
 * This example uses 16bit timer to trigger interrupt appr. every 250ms.
 * We calculate prescaler for cpu to get near 250ms.
 *-----------------------------------------------------------------------------
 */


#include <avr/io.h>
#include <avr/interrupt.h>

int timer_example()
{
	/* set pinb4 for output */
	DDRB |=(1<<PINB4);
	
	/* select and set prescaler to Timer/Counter1 Control Register 
	 * We have 2MHz cpu and 16bit timer. Timer can store 16^2-1 = 65535 bits.
	 * Clock signal goes around every 1/2000000 = 0.0000005 seconds.
	 * Therefore timer overflows every 65535 * 0.0000005= 0,0327 second.
	 * Goal is to make overflow near 250ms so we have to use prescaler.
	 * Simple equation: overflow_time = 65535 * prescaler/cpu, where prescaler is
	 * prescaler = cpu*(time/65535) ==> 2000 000 * (0,250/65535) = ~7,6.
	 * Closest prescaler value is 8 from datasheet Table 14-6. 
	 * Clock Select Bit Description which is bit 1 in TCCR1B, 0000 0010 (CS11). 
	 * With that we can get close enough, 262ms interrupt.
	 */
	TCCR1B |= (1<<CS11);
	
	/* Enable Overflow Interrupt in 16bit Timer.
	 * When this bit is written to one, and the I-flag in the Status Register 
	 * is set (interrupts globally enabled), the Timer/Countern Overflow interrupt 
	 * is enabled.
	 */
	TIMSK1 |=(1<<TOIE1); 
	
	/* enable global interrupt in Status Register 
	 * SREG |=(1<<0x80);
	 */
	sei(); 
	
	while(1) {}
	
}

	
/* overflow interrupt from timer triggered every 262m. */
ISR(TIMER1_OVF_vect)
{
	/* xor pinb4 for blink */
	PORTB ^=PINB4;
}
