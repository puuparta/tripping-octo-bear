/*
 * adc_example.c
 * ----------------------------------------------------------------------------
 * This is explanatory and educational version of Analog to Digital converter 
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
 * Title:	Analog to Digital converter (adc_example.c)
 * Created:	03.03.2013 22:30
 * Author:	Pasi Heinonen <pasi.heinonen@gmail.com>, https://twitter.com/pasihe
 *
 * A single conversion is started by writing a logical one to the 
 * ADC Start Conversion bit, ADSC. This bit stays high as long as the conversion 
 * is in progress and will be cleared by hardware when the conversion is 
 * completed. If a different data channel is selected while a conversion is in 
 * progress, the ADC will finish the current conversion before performing the 
 * channel change.
 
 * This example converts analog voltage to digital and puts result to port b.
 * in ATmega 16/32U4 Port F serves as analog inputs to the A/D Converter.
 *-----------------------------------------------------------------------------
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

int adc_example()
{
	/* set port b to output */
	DDRB = 0xFF;
	
	/* 8-bit or 10-bit results. we choose 10-bit.
	 * This will be set with ADC Multiplexer Selection Register.
	 * ADMUX Bit 5 – ADLAR: ADC Left Adjust Result. The ADLAR bit affects the presentation 
	 * of the ADC conversion result in the ADC Data Register. Write one to ADLAR to left 
	 * adjust the result. Otherwise, the result is right adjusted.
	 * from Table 24-4. Input Channel and Gain Selections can be seen that ADC0 channel is 
	 * selected by default and we use that now, ADC0 is PF0.
	 */
	ADMUX |=(1<<ADLAR);
	
	/* Sets highest voltage limit capacitor in a AREF-pin for filtering noise. 
	 * We choose REFS0. AVCC=5V with external capacitor at AREF pin. 
	 */
	ADMUX |=(1<<REFS0);

	/* Enable interrupts function in ADC */
	ADCSRA |=(1<<ADIE);

	/* because ADC is rather slow, we use prescaler determined by the internal/external 
	 * clock to set sample rate for ADC converter. By default, the successive approximation 
	 * circuitry requires an input clock frequency between 50kHz and 200 kHz to get 
	 * maximum resolution. If a lower resolution than 10 bits is needed, the input clock 
	 * frequency to the ADC can be higher than 200 kHz to get a higher sample rate.
	 * For instance if we have 1MHz crystal we calculate desired prescaler:
	 * 1 000 000/50 000 = 20, 1 000 000/200 000 = 5. Therefore we have to choose prescaler
	 * between 5...20. We choose 16, datasheet Table 24-5. ADC Prescaler Selections.
	 * Sample rate is then 1 000 000/16 = 62500 (62,5kHz). 
	 */
	ADCSRA |= (1<<ADPS2);
	
	/* Turn on the ADC feature. Bit 7 – ADEN: ADC Enable
	 * Writing this bit to one enables the ADC. By writing it to zero, the ADC is turned off.
	 */
	ADCSRA |= (1<<ADEN);
	
	// enable global interrupt
	sei();
		
	/* start conversion. Bit 6 – ADSC: ADC Start Conversion. */
	ADCSRA |=(1<<ADSC);

	while(1) {}
	
}

	
/* interrupt from ADC */
ISR(ADC_vect)
{
	/* Read the conversion result. The ADC Data Register – ADCL and ADCH.
	 * Because we chosen to use 10 bits we are reading from ADCH 
	 * Here we can do something with adc_result.
	 * for example writing to LCD
	 * char adc_result[4];
	 * itoa(ADCH,adc_result,10);
	 * send_to_lcd(adc_result);
	 */
	
	/* write analog-to-digital conversion directly to the port b */
	PORTB = ADCH;
	
	/* restart conversion. */
	ADCSRA |=(1<<ADSC);
}
