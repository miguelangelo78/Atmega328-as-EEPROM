#include <avr/pgmspace.h>
#include <Flash/Flash.h>

#ifndef DAT
FLASH_ARRAY(byte,DAT,
0b10101010,0b10,0x61

);
#endif

// ADDRESS: PD0-7 + PB0-4 = 12 PINS
// READ: PB5
// DATA: PB6-7 + PC0-5

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

#define _OE 0b100000
#define PORTB_LAST1 0b1000000
#define PORTB_LAST2 0b10000000

void setup() {
	DDRD = 0; // INPUT
	DDRB = 0; // 1 TRISTATE , INPUT
	DDRC = 0; // TRISTATE
}

void load(){
	// SET DATA DIRECTION:
	DDRC = 0b111111;
	bit_set(DDRB,PORTB_LAST1);
	bit_set(DDRB,PORTB_LAST2);
	
	uint16_t address = 0;
	byte data = 0;
	// TODO: READ ADDRESS
	while(!bit_get(PINB,_OE)){
		address = PIND;
		address+=bit_get(PINB,0b1)     << 8;
		address+=bit_get(PINB,0b10)    << 9;
		address+=bit_get(PINB,0b100)   << 10;
		address+=bit_get(PINB,0b1000)  << 11;
		address+=bit_get(PINB,0b10000) << 12;
		
		// LOAD DATA FROM FLASH USING ADDRESS:
		data = DAT[address];
		
		// OUTPUT LOADED DATA TO BUS:
		bit_write(data & 0x1,PORTB,PORTB_LAST1);
		bit_write(data & 0x2,PORTB,PORTB_LAST2);
		PORTC = data>>2;
		
		// WAIT TILL IT'S NOT READING ANYMORE
	}
	
	// RESET ALL I/O PINS
	DDRC  = 0;
	bit_clear(DDRB,PORTB_LAST1);
	bit_clear(DDRB,PORTB_LAST2);
	PORTC = 0;
	bit_write(0,PORTB,PORTB_LAST1);
	bit_write(0,PORTB,PORTB_LAST2);
}

void loop() {
	// Check if it's reading:
	if(!bit_get(PINB,_OE))
	load(); // If it's reading, fetch the address and output the data and hold it till it's not reading anymore
}
