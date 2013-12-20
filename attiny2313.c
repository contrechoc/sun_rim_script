/*

solar dress script.

The signal arrives from a atmega328 using TX DX

The signal is 8 bytes, representing 64 on off bits.

cols and rows depends on the connections and the groupings

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU  8000000L 

#include <util/delay.h>

/* Prototypes */

void InitUART (unsigned char baudrate);
unsigned char ReceiveByte (void);
void TransmitByte (unsigned char data);
void fancy();

#define TIMER1_PRESCALE_64 2
 
#define HIGH 0
#define LOW 1

unsigned char col = 0;
unsigned char row = 0;
volatile unsigned char counter = 0;
volatile unsigned char counterPrescaler = 0;
volatile unsigned char counterP = 2;

unsigned char  cols[8] = { 7,  6, 5, 4, 3 , 2, 1, 0 };
unsigned char  rows[8] = {15, 14, 13, 12, 11, 10, 9, 8 };

unsigned char leds[8] ;

unsigned char charCounter = 0;
unsigned char tChar = 0;

int counterSign = 1;

void delay_ms( uint16_t milliseconds);
void clearLeds() ;
void setPattern(char inChar);
void digitalWrite(unsigned char pinNum, unsigned char highLow) ;
void circle(char speed);

void digitalWrite(unsigned char pinNum, unsigned char highLow) {
	if ( highLow ==  1 )
	{
		 switch ( pinNum ) {
			case 15: PORTD &= ~_BV(PD1); break;
			case 14: PORTA &= ~_BV(PA1); break;
			case 13: PORTA &= ~_BV(PA0); break;
			case 12: PORTD &= ~_BV(PD2); break;
			case 11: PORTD &= ~_BV(PD3); break;
			case 10: PORTD &= ~_BV(PD4); break;
			case 9: PORTD &= ~_BV(PD5); break;
			case 8: PORTD &= ~_BV(PD6); break;

			case 7: PORTB &= ~_BV(PB0); break;
			case 6: PORTB &= ~_BV(PB1); break;
			case 5: PORTB &= ~_BV(PB2); break;
			case 4: PORTB &= ~_BV(PB3); break;
			case 3: PORTB &= ~_BV(PB4); break;
			case 2: PORTB &= ~_BV(PB5); break;
			case 1: PORTB &= ~_BV(PB6); break;
			case 0: PORTB &= ~_BV(PB7); break;
 
		 }
	 }
	 else
	 {
		 switch ( pinNum ) {
			case 15: PORTD |= (1<<PD1); break;
			case 14: PORTA |= (1<<PA1); break;
			case 13: PORTA |= (1<<PA0); break;
			case 12: PORTD |= (1<<PD2); break;
			case 11: PORTD |= (1<<PD3); break;
			case 10: PORTD |= (1<<PD4); break;
			case 9: PORTD |= (1<<PD5); break;
			case 8: PORTD |= (1<<PD6); break;

			case 7: PORTB |= (1<<PB0); break;
			case 6: PORTB |= (1<<PB1); break;
			case 5: PORTB |= (1<<PB2); break;
			case 4: PORTB |= (1<<PB3); break;
			case 3: PORTB |= (1<<PB4); break;
			case 2: PORTB |= (1<<PB5); break;
			case 1: PORTB |= (1<<PB6); break;
			case 0: PORTB |= (1<<PB7); break;

		 }
	 }
}

void circle(char speed){

char c = 0;
char cc = 0;
for (  c = 0; c < 64; c++){
	cc = (c%8);
	clearLeds();
  	leds[c/8] = 0xFF - (1<<cc);
		delay_ms(speed);
 	}

}

void setPattern(char inChar) {

/* ----> example just counting around the ring
counter++;
if ( counter > 64 ) counter = 0;

unsigned char cc = (counter%8);
clearLeds();
unsigned char i = 0;

for (  i = 0; i < 8; i++)
  leds[counter/8] = 0xFF - (1<<cc);
*/
 
  leds[charCounter%8] = 0xFF - inChar;
	charCounter++;

}


void clearLeds() {
unsigned char i = 0;
  for (  i = 0; i < 8; i++)
      leds[i] = 0xFF;
}


void delay_ms( uint16_t milliseconds)
{
   for( ; milliseconds > 0; milliseconds--)
   {
      _delay_ms( 1);
   }
}

int main (void)
{

	DDRB = 0xFF;			// Set output.
	DDRD = 0xFF;
	DDRA = 0xFF;

	PORTD = 0x00;
	PORTB = 0x00;
	PORTA = 0x00;

	clearLeds();

  TCCR0B = (1 << WGM02) | counterP; //WGNM12 bit3 of tccr1b timer control register
  OCR0A = (uint16_t)2000;//output compare register comined with WGM02 set
  TIMSK |= 1 << OCIE0A;   // Output Compare Interrupt Enable (timer 1, OCR1A)
  
  	sei();
  	InitUART (51);  //for communicating with arduino at 16000 attiny2313 on 8000

 
unsigned char i3 = 0;
for (  i3 = 0; i3 < 5; i3++)
circle(5-i3);
 

	 while (1)
    {
unsigned char i = 0;

  for (  i = 0; i < 8; i++){
	unsigned char inCharR = ReceiveByte (); 
        setPattern(inCharR);
	delay_ms(5);
 }
 


  //for (  i = 0; i < 20; i++)
//	delay_ms(50);



	/*
		if ( inChar =='f' | inChar =='F' )
		{
		 	fancy();
		}
*/

	 if ( (1==0) && (counterPrescaler%50 == 0)  )
	 	{
			cli();
			TCCR0B = (1 << WGM02) | counterP; //WGNM12 bit3 of tccr1b timer control register
			OCR0A = (uint16_t)2000;//output compare register comined with WGM02 set
			TIMSK |= 1 << OCIE0A;   // Output Compare Interrupt Enable (timer 1, OCR1A)
			counterP++;
  			if ( counterP > 4 ) counterP = 2;
			sei();
			delay_ms(200);
	 }
 
	}
}

 

// Interrupt routine
SIGNAL( TIMER0_COMPA_vect   )  {

	digitalWrite( rows[row], LOW );

  	row++;
  	if (row == 8)
    	row = 0;

	unsigned char col = 0;
  	for (  col = 0; col < 8; col++) {

	    if (  (leds[row]  & (1 << col )) == HIGH )
	     	digitalWrite( cols[col], LOW);  // Turn on this led
	    else
	     	digitalWrite( cols[col], HIGH); // Turn off this led
	  	}
 
	digitalWrite( rows[row],  HIGH);
}

/* Initialize UART */
void InitUART (unsigned char baudrate)
{
  /* Set the baud rate */
  UBRRL = baudrate;

  /* Enable UART receiver and NOT transmitter */
  UCSRB = (1 << RXEN);// | (1 << TXEN);

  /* set to 8 data bits, 1 stop bit */
  UCSRC = (1 << UCSZ1) | (1 << UCSZ0);

}

/* Read and write functions */
unsigned char ReceiveByte (void)
{

  /* Wait for incomming data */
  while (!(UCSRA & (1 << RXC))   );

  /* Return the data */

  return UDR;

}

void TransmitByte (unsigned char data)
{
  /* Wait for empty transmit buffer */
  while (!(UCSRA & (1 << UDRE)));
//	blinkEm(100,0);
  /* Start transmittion */
  UDR = data;
}

