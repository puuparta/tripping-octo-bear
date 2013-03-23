/*
 * lcd_sample.c
 *
 * Created: 11.3.2013 21:40:47
 *  Author: pheinone
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/************************************************************************/
/* Porttiasetukset                                                      */
/************************************************************************/
// projektissa k�ytet��n 4bittist� lcd interfacea. 
// LCD:n datav�yl� eli DB4-DB7 on kytketty ATmegan pinneihin PORTB0-PORTB3
// RS on kytketty PORTB4, RW PORTB5, EN PORTB6
#define LCD_PORT PORTB
#define LCD_PORT_DIR DDRB
#define EN 6
#define RW 5
#define RS 4

/* MCU ohjelmointi on 80% datasheetien tulkintaa ja 20% kahvinjuontia.
 * Komentojen sek� varsinaisen merkkidatan kirjoitus LCD:lle on 
 * ohjelmoitu HD44780 datasheetin sivun 33 kuvan "Example of 4-Bit Data 
 * Transfer Timing Sequence" mukaan. Kuvassa RS signaali on 1 eli 
 * kirjoitetaan dataa. Komennon kirjoittamisessa RS on vastaavasti 0.
 * HD44780 datasheet: http://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 */

/* esitell��n prototyypit, ettei tule void paluuarvoja miss� ei pit�isi.
 * avr-gcc pillastuu ilman n�it�. 
 */
void lcd_init();
void write_cmd(unsigned char cmd);
void write_data(unsigned char data);
void signal_e();
void wait_bf();
void write_string(char *str);
void i2c_init2();
unsigned char read_temp();

int main()
{
	// portin suunnaksi l�ht�, tuloa tarvitaan esim. kun kysyt��n 
	// LCD:lt� busyflagia (BF)
	LCD_PORT_DIR = 0xff;
	
	// alustetaan lcd
	lcd_init();
	
	// alustetaan i2c v�yl�
	i2c_init2();
	
	write_cmd(0x01); // n�yt�n tyhjennys
	
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
	
	while(1)
	{
		write_cmd(1<<7 | 10); // siirryt��n positioon 10 ekalle riville
		unsigned char lampotila_sensorilta = read_temp();
		unsigned char buffer[7];
		unsigned char debug_buffer[7]; 
		itoa(lampotila_sensorilta, debug_buffer, 10);
		
		/* TC74 datasheetin Taulukossa kuvataan l�mp�tilan digitalisointi
		 * TABLE 4-4: TEMPERATURE-TO-DIGITAL VALUE CONVERSION 
		 * -65...+130C
		 * 0-127 = + alue ja 128-256 on - alue
		 */
		if (lampotila_sensorilta > 127) {
			write_data('-');
			lampotila_sensorilta = ~lampotila_sensorilta+1;
		}
		else if(lampotila_sensorilta>0)
		{
			write_data('+');
		}
		itoa(lampotila_sensorilta, buffer, 10);
		write_string(buffer);
		write_data(0b11011111); //aste-merkki
		write_data('C');
		
		write_cmd(1<<7 | 18); // siirryt��n positioon 1 toiselle riville
		write_string(debug_buffer);
		_delay_ms(1000);
	}
}

/************************************************************************/
/* LCD:n alustus                                                        */
/************************************************************************/
void lcd_init()
{
	/* 4bit moodiin resetointi 
	 * datasheet sivu 46: Figure 24, 4-Bit Interface
	 * Resetointibitit voi l�hett�� my�s yhten� 4 bitin pakettina
	 * kerrallaan mutta k�ytet��n t�ss� samaa funktiota kuin
	 * LCD:n asetusbiteiss�. Lopputulos on sama.
	 */
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x03);
	write_cmd(0x02);
		
	/* LCD:n asetusbitit.
	 * datasheet sivu 42: Table 12 4-Bit Operation ja sivu 24 Instructions
	 * 4bit moodissa vain DB7-DB4 on k�yt�ss�.
	 */
	write_cmd(0x20); // 4bit mode on annettava viel� kerran
	write_cmd(0x0C); // n�ytt� p��lle, kursori piiloon.
	//demomode: 
	//write_cmd(0x0F); // n�ytt� p��lle ja demon vuoksi kursori vilkkumaan.
	write_cmd(0x06); // entry moodi: kirjoitus ja kursorisuunta oikealle
	write_cmd(0x01); // n�yt�n tyhjennys

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

/************************************************************************/
/* Alustetaan I2C-v�yl�                                                 */
/************************************************************************/
void i2c_init2()
{
	DDRD = DDRD & 0xfc; // scl & sda portit tuloiksi
	PORTD |= (1<<PD1); // pull-up bit 1, SDA
	PORTD |= (1<<PD0); // pull-up bit 0, SCL
	TWSR = 0x00; //prescaler=1
	TWBR = 0x04; // clockfreq = 2mhz/(16+2*4*4^1) = ~41,5khz	
}

/************************************************************************/
/* luetaan i2c-v�yl�lt� TC74 l�mp�tila                                  */
/* TC74 A3, 5.0V AT			                                            */
// model: 11103GU                                                       */
/************************************************************************/
unsigned char read_temp()
{
	char vastaus;
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	
	// TC74 osoite v�yl�ll�
	TWDR = 0x97;
	
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	
	vastaus = TWDR;
	return vastaus;
}

