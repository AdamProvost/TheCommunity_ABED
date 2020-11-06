#ifndef F_CPU
#define F_CPU 16000000UL // telling controller crystal frequency (16 MHz AVR ATMega328P)
#endif

// SPI INTERFACE DEFINES
#define MOSI 3 // MOSI it's PORT B, PIN 3
#define MISO 4 // MISO it's PORT B, PIN 4
#define SCK 5 // SCK it's PORT B, PIN 5
#define SS 2 // SS it's PORT B, PIN 2

// RESET THE DISPLAY
#define RST 0 // RESET it's PORT B, PIN 0

//DISPLAY MODE SELECT - Input to select either command/address or data input.
#define DC 1 // DC it's PORT B, PIN 1

//array of distance levels [0..40]
unsigned char Dist_value[8] = {0,0,0,0,0,0,0,0};

// array of distance level signs [1..8]
static const unsigned char LVL_DIST[8][8] =
{
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},	 // 1
	{ 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0},	 // 2
	{ 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0},	 // 3
	{ 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},	 // 4
	{ 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8},	 // 5
	{ 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC},	 // 6
	{ 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE},   // 7
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // 8
};
// array of digits [0..9] and sign >
static const unsigned char DIG[11][8] =
{
	{0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00} //  0
	,{0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00} //  1
	,{0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00, 0x00} //  2
	,{0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, 0x00, 0x00} //  3
	,{0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00, 0x00} //  4
	,{0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00, 0x00} //  5
	,{0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00, 0x00} //  6
	,{0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00, 0x00} //  7
	,{0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00} //  8
	,{0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x00} //  9
	,{0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00} //  >
};
// codes array of words "MIN_MAX:"
static const unsigned char MIN_MAX[25] =
{
	0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x00, 0x0E, 0x05, 0x0E, 0x00, 0x0D, 0x02, 0x0D,
	0xE0, 0x10, 0xE0, 0x10, 0xE0, 0x00, 0xF0, 0x00, 0xF0, 0x20, 0x40, 0xF0
};

// codes array of word "DIST:"
const unsigned char DIST[15] =
{
	0x3E, 0x22, 0x1C, 0x00, 0x3A, 0x00, 0x24, 0x2A, 0x12, 0x00, 0x02, 0x3E, 0x02, 0x00, 0x14 
};

// codes array of word "CM:"
const unsigned char CM[9] =
{
	0x1C, 0x22, 0x22, 0x00, 0x3C, 0x02, 0x3C, 0x02, 0x3C
};


//pulse length
volatile unsigned short hc_sr04_cnt;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

// Port Initialization
void Port_Init()
{
	DDRB = (1<<MOSI)|(1<<SCK)|(1<<SS)|(1<<RST)|(1<<DC);// Set MOSI,SCK,SS,RST,DC as output, all others input
	PORTB |= (1<<RST);// Set RST pin as high
	PORTB |= (1<<SS);// Set SS pin as high - Display is Disable
	
	DDRD=0b00001000; //	Set all pins of the PORTD as output, PORTD3 as input.
	PORTD=0b00000000; // Set all pins of PORTD low which turns it off.
 
	EIMSK |= (1<<INT0); //enable external interrupt
	EICRA |= (1<<ISC00); // Any logical change on INT0 generates an interrupt request.

	//Config and enable Timer1
	TCCR1B = (0<<CS12)|(1<<CS11)|(1<<CS10); 
}

ISR( INT0_vect )
{
	
	if(PIND & (1 << 2)) {TCNT1 = 0;}/* Clear Timer counter */
	else  {hc_sr04_cnt = TCNT1;}
		

}

unsigned char hc_sr04_meas( void )
{
	
	hc_sr04_cnt = 0;
	
	// transmit at least 10 us trigger pulse to the HC-SR04 Trig Pin.
	PORTD |=  (1 << 3);
	_delay_us( 10 );
	PORTD &= ~(1 << 3);


	while( hc_sr04_cnt == 0 );
	
	if (hc_sr04_cnt<3700) {return 0.000004 * hc_sr04_cnt/2 * 34300;} else {return 41;}
	
}

// SPI Initialization
void SPI_Init()
{
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//Enable SPI, Set as Master, Set Prescaler as Fosc/16 in SPI control register
}

/*
//Display Enable
void SPI_SS_Enable()
{
	PORTB &= ~(1<<SS); // Enable the SS pin to logic 0
}

//Display Disable
void SPI_SS_Disable()
{
	PORTB |= (1<<SS); // Disable the SS pin to logic 1
}

//Function to send data into the display buffer
void SPI_Tranceiver (unsigned char data)
{
	SPDR = data;                       //Load data into the buffer
	while(!(SPSR & (1<<SPIF) ));       //Wait until transmission complete
}

// Reset the Display at the beginning of initialization
void Display_Reset()
{
	PORTB &= ~(1<<RST);
	_delay_ms(100);
	PORTB |= (1<<RST);
}

//Command write function
void Display_Cmnd(unsigned char data)
{
	PORTB &= ~(1<<DC); // make DC pin to logic 0 for command operation
	SPI_Tranceiver(data);  // send data on data register
	PORTB |= (1<<DC); // make DC pin to logic high for data operation
}

//Initialization of Display
void Display_init()
{
	Display_Reset();  // reset the display
	Display_Cmnd(0x21);  // command set in addition mode
	Display_Cmnd(0xC0);  // set the voltage by sending C0 means VOP = 5V
	Display_Cmnd(0x07);  // set the temp. coefficient to 3
	Display_Cmnd(0x13);  // set value of Voltage Bias System
	Display_Cmnd(0x20);  // command set in basic mode
	Display_Cmnd(0x0C);  // display result in normal mode
}

// set the column and row  to the position of displaying result on LCD Display
void Display_SetXY(unsigned char x, unsigned char y)
{
	Display_Cmnd(0x80|x); // column (0-83)
	Display_Cmnd(0x40|y); // row (0-5)
}

// Function to clear the digital sign
void Off_Dig(unsigned char x, unsigned char y)
{
	Display_SetXY(x,y); // Set the address of position on display (top row)
	for (int index=0; index<5; index++)
	{SPI_Tranceiver(0);} //Load data into the buffer of display (clear top part of digital sign)
}

// Function to display digital sign
void Display_Dig(unsigned char dig, unsigned char x, unsigned char y)
{
	Display_SetXY(x,y);// Set the address of position on display (top row)
	for (int index=0; index<5; index++)
	{
		SPDR = DIG[dig][index];//DIG[dig][index]; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
}

void Display_level()
{
	for (int i=0; i<8; i++)
	{
		unsigned int lvl = Dist_value[i] / 8;
		
		for (int index=1; index<5; index++)
		{
			Display_SetXY(15+i*8,index); // Set the address of position on display (up row)
			for (int index1=0; index1<8; index1++)
			{SPI_Tranceiver(0);}
		}
				
		for (int index=0; index<lvl; index++)
		{
			Display_SetXY(15+i*8,5-index); // Set the address of position on display (up row)
			for (int index1=0; index1<8; index1++)
			{
				SPDR = LVL_DIST[7][index1]; // Load codes array data into the buffer of display
				while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
			}
		}	
		
		if ((Dist_value[i]-lvl * 8) > 0)
		{
			Display_SetXY(15+i*8,5-lvl);
			for (int index1=0; index1<8; index1++)
			{
				SPDR = LVL_DIST[Dist_value[i]-lvl * 8][index1]; // Load codes array data into the buffer of display
				while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
			}
		}
		_delay_ms(10);
	}
}


void Display_Dist(unsigned char buffer)
{
	
	unsigned char temp_int_0,temp_int_1; // single digits, double digits, triple digits, quarter digits
	
	if (buffer==41)
	{
		 Display_Dig(10,35,0); // display 1 digit of light level
		 Display_Dig(4,42,0); // display 1 digit of light level
		 Display_Dig(0,48,0); // display 2 digit of light level
	}
	else
	{
		Off_Dig(35,0);
		temp_int_0 = buffer % 100 / 10; // double-digit
		temp_int_1 = buffer % 10; // single-digit
		
		if(temp_int_0 > 0) // if result is quarter-digit number
		{
 			Display_Dig(temp_int_0,42,0); // display 1 digit of light level
 			Display_Dig(temp_int_1,48,0); // display 2 digit of light level
 		}
		else
		{
			if(temp_int_1 > 0) // if result is triple-digit number
			{
				Off_Dig(42,0); // clear 1 sign of number
				Display_Dig(temp_int_1,48,0); // display 1 digit of light level
			}
		}
	}
}

// Function to display words 
void Display_label()
{
	// Word "DIST"
	Display_SetXY(20,0); // Set the address of position on display (up row)
	for (int index=0; index<15; index++)
	{
		SPDR = DIST[index]; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
	
	// Word "CM"
	Display_SetXY(55,0); // Set the address of position on display (up row)
	for (int index=0; index<9; index++)
	{
		SPDR = CM[index]; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
		

	// Word "MIN_MAX"
	Display_SetXY(0,1); // Set the address of position on display (up row)
	for (int index=0; index<13; index++)
	{
		SPDR = MIN_MAX[index]; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
	Display_SetXY(0,5); // Set the address of position on display (up row)
	for (int index=13; index<25; index++)
	{
		SPDR = MIN_MAX[index]; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
	
	// LINE_Y
	for (int index=1; index<6; index++)
	{
		Display_SetXY(14,index); // Set the address of position on display (up row)
		SPDR = 0xFF; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
	
	// LINE_X
	Display_SetXY(14,5); // Set the address of position on display (up row)
	SPDR = 0xFF; // Load codes array data into the buffer of display
	while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
			
	Display_SetXY(15,5); // Set the address of position on display (up row)			
	for (int index=0; index<69; index++)
	{
		
		SPDR = 0x80; // Load codes array data into the buffer of display
		while(!(SPSR & (1<<SPIF) )); // Wait until transmission complete
		_delay_ms(10);
	}
}

// Clear the Display
void Display_Clear()
{
	PORTB |= (1<<DC); // make DC pin to logic high for data operation
	for (int k=0; k<=503; k++){	SPI_Tranceiver(0x00);}
	PORTB &= ~(1<<DC);// make DC pin to logic zero for command operation
}
*/
void Add_Dist_value(unsigned char value)
{
    for (int i=7; i>0; i--)
    Dist_value[i] = Dist_value[i-1];
    Dist_value[0] = value;
}


int main(void)
{
	unsigned char sm;
	
	
	
	Port_Init(); // Port Initialization
	/*SPI_Init(); // SPI Initialization
	SPI_SS_Enable(); // Display Enable
	Display_init(); // Display initialization
	Display_Clear();// Display Clear
	Display_label(); // Display Interface
    */
	
	UART_Init(9600);  /* Initialize UART at 9600 baud rate */
	
	sei();
	// Infinity loop
	while (1)
	{
		sm = hc_sr04_meas();
		UART_Printf("TCNT1:%5d\n\r",TCNT1);     // Send the value on UART
		UART_Printf("SM:%5d\n\r",sm);     // Send the value on UART
		if (sm > 40) {sm=40;Display_Dist(41);} else {Display_Dist(sm);}
		Add_Dist_value(sm);
		Display_level();
		_delay_ms( 200 );
	}
}