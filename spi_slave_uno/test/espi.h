#define F_CPU 16000000UL			/* Define CPU Frequency 8MHz */
#include <avr/io.h>			/* Include AVR std. library file */
//#include <util/delay.h>			/* Include Delay header file */
#include <stdio.h>			/* Include std. i/p o/p file */
#include <string.h>			/* Include string header file */
//#include "LCD_16x2_H_file.h"		/* Include LCD header file */
//#include "SPI_Slave_H_file.h"		/* Include SPI slave header file */

#define     SS      DDB2
#define     MOSI    DDB3
#define     MISO    DDB4
#define     SCK     DDB5

#define     CPOLE_VALUE       0
#define     CPHASE_VALUE      0
#define     SPR1_VALUE        0
#define     SPR0_VALUE        1
#define     DORD_VALUE        0   //MSB transmitted first.

void SPI_Init();
uint8_t SPI_Receive();
uint16_t SPI_recieve16();