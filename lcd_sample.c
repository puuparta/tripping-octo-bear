/*
 * lcd_sample.c
 *
 * Created: 11.3.2013 21:40:47
 *  Author: pheinone
 */ 

#include <avr/io.h>
#include <util/delay.h>

/************************************************************************/
/* Porttiasetukset                                                      */
/************************************************************************/
// projektissa käytetään 4bittistä lcd interfacea. 
// LCD:n dataväylä eli DB4-DB7 on kytketty ATmegan pinneihin PORTB0-PORTB3
// RS on kytketty PORTB4, RW PORTB5, EN PORTB6
#define LCD_PORT PORTB
#define LCD_PORT_DIR DDRB
#define EN 6
#define RW 5
#define RS 4

/* MCU ohjelmointi on 80% datasheetien tulkintaa ja 20% kahvinjuontia.
 * Komentojen sekä varsinaisen merkkidatan kirjoitus LCD:lle on 
 * ohjelmoitu HD44780 datasheetin sivun 33 kuvan "Example of 4-Bit Data 
 * Transfer Timing Sequence" mukaan. Kuvassa RS signaali on 1 eli 
 * kirjoitetaan dataa. Komennon kirjoittamisessa RS on vastaavasti 0.
 * HD44780 datasheet: http://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 */

int main()
{
	// portin suunnaksi lähtö, tuloa tarvitaan esim. kun kysytään 
	// LCD:ltä busyflagia (BF)
	LCD_PORT_DIR = 0xff;
	
	// alustetaan lcd
	lcd_init();
	
	// kirjoitetaan "Lämpötila: 24 C"
	write_data('L');
	write_data(0b11100001); //ä
	write_data('m');
	write_data('p');
	write_data(0b11101111); //ö
	write_data('t');
	write_data('i');
	write_data('l');
	write_data('a');
	write_data(':');
	
	char* lampotila_sensorilta ="24.7";
	write_string(lampotila_sensorilta);
	write_data(0b11011111); //aste-merkki
	write_data('C');
	
	while(1);
}

/************************************************************************/
/* LCD:n alustus                                                        */
/************************************************************************/
void lcd_init()
{
	/* 4bit moodiin resetointi 
	 * datasheet sivu 46: Figure 24, 4-Bit Interface
	 * Resetointibitit voi lähettää myös yhtenä 4 bitin pakettina
	 * kerrallaan mutta käytetään tässä samaa funktiota kuin
	 * LCD:n asetusbiteissä. Lopputulos on sama.
	 */
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x02);
		
	/* LCD:n asetusbitit.
	 * datasheet sivu 42: Table 12 4-Bit Operation ja sivu 24 Instructions
	 * 4bit moodissa vain DB7-DB4 on käytössä.
	 */
	write_cmd(0x20); // 4bit mode on annettava vielä kerran
	write_cmd(0x0F); // näyttö päälle ja demon vuoksi kursori vilkkumaan.
	write_cmd(0x06); // entry moodi: kirjoitus ja kursorisuunta oikealle 

}

/************************************************************************/
/* Lähetetään komento (instruction) väylälle ja signaloidaan E          */
/* jotta komento siirtyy LCD:n komentorekisteriin (IR)                  */
/* RS=0, valittuna IR eli komentorekisteri.                             */
/************************************************************************/
void write_cmd(unsigned char cmd)
{
	wait_bf();  // ei välttämätön
	LCD_PORT = (cmd>>4 & 0x0F); // eniten merkitsevät 4bittiä
	signal_e();
	
	LCD_PORT = (cmd & 0x0F); // vähiten merkitsevät 4bittiä
	signal_e();
}

/************************************************************************/
/* Lähetetään data väylälle ja signaloidaan E jotta data siirtyy        */
/* LCD:n datarekisterii (DR). RS=1, eli valittuna datarekisteri.        */
/************************************************************************/
void write_data(unsigned char data)
{
	wait_bf(); //ei välttämätön
	LCD_PORT = (data>>4 & 0x0F)|(1<<RS); // eniten merkitsevät 4bittiä
	signal_e();
	
	LCD_PORT = (data & 0x0F)|(1<<RS); // vähiten merkitsevät 4bittiä
	signal_e();
}

/************************************************************************/
/* lähetetään E-signaali, väylällä oleva tieto kirjoitetaan lcd:lle     */ 
/* E-signaalin laskevalla reunalla.										*/
/************************************************************************/
void signal_e()
{
	LCD_PORT |= (1<<EN);
	_delay_ms(5);
	LCD_PORT &= ~(1<<EN); // komennetaan LCD:tä lukemaan väylältä
}

/************************************************************************/
/* Tarkistetaan ja odotetaan, että edellinen komento suoritetaan        */
/* luetaan tämä statusrekisterin ylimmästä bitistä, BF				    */
/************************************************************************/
void wait_bf()
{
	LCD_PORT_DIR = 0xF0; // datapinnit tuloksi, koska luetaan busy flagia
	LCD_PORT |= (1<<RW); //RW=1
	LCD_PORT &= ~(1<<RS);//RS=0

	while (LCD_PORT >= 0x80) //0x80 on Busy flag, datasheet sivu 24.
	{
		 signal_e();
	}
	LCD_PORT_DIR = 0xFF; // asetetaan kaikki pinnit takaisin lähdöiksi
}

/************************************************************************/
/* Kirjoittaa merkkijonon merkki kerrallaan                             */
/************************************************************************/
void write_string(char *str)
{
	while(*str > 0)
	{
		write_data(*str++);
	}
}

