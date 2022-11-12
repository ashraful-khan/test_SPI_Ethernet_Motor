#include    "spi.h"

void SPI_Init()					/* SPI Initialize function */
{
	DDRB &= ~((1<<MOSI)|(1<<SCK)|(1<<SS));  /* Make MOSI, SCK, SS as input pins */
	DDRB |= (1<<MISO);						/* Make MISO pin as output pin */
	//SPCR = (1<<SPE) | (0<<MSTR);			/* Enable SPI in slave mode */
	//SPCR = 0b01000001;			/* Enable SPI in slave mode */

  SPCR |= (1 << SPE) | (DORD_VALUE << DORD) | (0 << MSTR) | (CPOLE_VALUE << CPOL) | (CPHASE_VALUE << CPHA) | (SPR1_VALUE << SPR1) | (SPR0_VALUE << SPR0);

	SPSR &= ~(1<<SPI2X);  /* Disable speed doubler */
}

uint8_t SPI_Receive()			/* SPI Receive data function */
{
	SPDR = NULL;
	while(!(SPSR & (1<<SPIF)));	/* Wait till reception complete */
	return(SPDR);			/* Return received data */
}


uint16_t SPI_recieve16() {
    union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
    //in.val = data;
    if (!(SPCR & _BV(DORD))) {
      //SPDR = in.msb;
      //SPDR = NULL;
      asm volatile("nop"); // See transfer(uint8_t) function
      while (!(SPSR & _BV(SPIF))) ;
      out.msb = SPDR;
      //SPDR = in.lsb;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.lsb = SPDR;
    } else {
      //SPDR = in.lsb;
      //SPDR = NULL;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.lsb = SPDR;
      //SPDR = in.msb;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.msb = SPDR;
    }
    return out.val;
  }