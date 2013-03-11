/*
 * lcd_sample.c
 *
 * Created: 11.3.2013 21:40:47
 *  Author: pheinone
 */ 

#include <avr/io.h>
#include <util/delay.h>

// Porttiasetukset
// projektissa k�ytet��n 4bittist� lcd interfacea. 
// LCD:n datav�yl� eli DB4-DB7 on kytketty ATmegan pinneihin PORTB0-PORTB3
// RS on kytketty PORTB4, RW PORTB5, EN PORTB6
#define LCD_PORT PORTB
#define LCD_PORT_DIR DDRB
#define EN 6
#define RW 5
#define RS 4

int main()
{
	// portin suunnaksi l�ht�, tuloa tarvitaan esim. kun kysyt��n 
	// LCD:lt� busyflagia (BF)
	LCD_PORT_DIR = 0xff;
	
	// alustetaan lcd
	lcd_init();
	
	// kirjoitetaan "L�mp�tila: 24 C"
	write_data('L');
	write_data(0b11100001); //�
	write_data('m');
	write_data('p');
	write_data(0b11101111); //�
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
	// 4bit moodiin resetointi, HD44780 datasheet 
	// http://www.sparkfun.com/datasheets/LCD/HD44780.pdf
	// datasheet sivu 46: Figure 24, 4-Bit Interface
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x02);
		
	// alustetaan halutuin konfiguroinnein
	// datasheet sivu 42: Table 12 4-Bit Operation ja sivu 24 Instructions
	write_cmd(0x02);
	write_cmd(0x08);
	write_cmd(0x01);
	write_cmd(0x06);
	write_cmd(0x0C);
}

/************************************************************************/
/* L�hetet��n komento (instruction) v�yl�lle ja signaloidaan E          */
/* jotta komento siirtyy LCD:n komentorekisteriin (IR)                  */
/* RS=0, valittuna IR eli komentorekisteri.                             */
/************************************************************************/
void write_cmd(unsigned char cmd)
{
	wait_bf();  // ei v�ltt�m�t�n
	LCD_PORT = (cmd>>4 & 0x0F); // eniten merkitsev�t 4bitti�
	signal_e();
	
	LCD_PORT = (cmd & 0x0F); // v�hiten merkitsev�t 4bitti�
	signal_e();
}

/************************************************************************/
/* L�hetet��n data v�yl�lle ja signaloidaan E jotta data siirtyy        */
/* LCD:n datarekisterii (DR). RS=1, eli valittuna datarekisteri.        */
/************************************************************************/
void write_data(unsigned char data)
{
	wait_bf(); //ei v�ltt�m�t�n
	LCD_PORT = (data>>4 & 0x0F)|(1<<RS); // eniten merkitsev�t 4bitti�
	signal_e();
	
	LCD_PORT = (data & 0x0F)|(1<<RS); // v�hiten merkitsev�t 4bitti�
	signal_e();
}

/************************************************************************/
/* l�hetet��n E-signaali, v�yl�ll� oleva tieto kirjoitetaan lcd:lle     */ 
/* E-signaalin laskevalla reunalla.										*/
/************************************************************************/
void signal_e()
{
	LCD_PORT |= (1<<EN);
	_delay_ms(5);
	LCD_PORT &= ~(1<<EN); // komennetaan LCD:t� lukemaan v�yl�lt�
}

/************************************************************************/
/* Tarkistetaan ja odotetaan, ett� edellinen komento suoritetaan        */
/* luetaan t�m� statusrekisterin ylimm�st� bitist�, BF				    */
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
	LCD_PORT_DIR = 0xFF; // asetetaan kaikki pinnit takaisin l�hd�iksi
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

