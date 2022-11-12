#if 0
#include <Arduino.h>
#include "spi.h"
#include <util/delay.h>

int main(void)
{
	//uint8_t data;
	uint16_t data;
	//char buffer[5];
	
    //uart_init();
	SPI_Init();
    while (1)			/* Receive count continuous */
	{
		data = SPI_recieve16();
		Serial.println(data, HEX);
		//printf("0x%04X   \n", data);
		_delay_ms(100);
	}
}

#endif

#if 1

#include <Arduino.h>
#include <SPI.h>
#include "TemplateHeader.h"

/* Globals */
bool spiMessageToBePrinted = false;
uint8_t readBuffer [100];
bool isStringMessage = true;

volatile bool controlFlag;
volatile byte pos = 1;
volatile boolean process_it = false;
volatile byte messageSize = 0;

/* This needs to be put before setup or in separate header file */
template <typename T> T serialPrintBinary(T x, bool usePrefix = true)
{
  if(usePrefix) Serial.print("0b");
  for(int i = 0; i < 8 * sizeof(x); i++)
  {
    Serial.print(bitRead(x, sizeof(x) * 8 - i - 1));
  }
  Serial.println();
  return x;
}

void setup() {
  Serial.begin(115200); // opens and configures the USB serial port for baudrate 115200
  Serial.println("SPI Slave Communication setup");
  /* This does some magic which is necessary on the Nano Every */
  SPI.begin();
  /* Enable SPI, put it in Slave mode, set LSB first */
  SPI0.CTRLA = (~SPI_DORD_bm & (SPI_ENABLE_bm & (~SPI_MASTER_bm)));  
  /* disable "Slave Select Disable" --> to ensure it will stay slave */
  SPI0.CTRLB &= ~(SPI_SSD_bm); 
  /* Set Mode 0 (is default though) */
  SPI0.CTRLB |= (SPI_MODE_0_gc); 
  SPI0.INTCTRL = SPI_IE_bm;        /* SPI Interrupt enable */
  
  Serial.println("SPI registers on ATmega4809 were set: ");
  Serial.print("SPI0_CTRLA: ");
  serialPrintBinary<byte>(SPI0_CTRLA);
  Serial.print("SPI0_CTRLB: ");
  serialPrintBinary<byte>(SPI0_CTRLB);
  Serial.print("SPI0_INTCTRL: ");
  serialPrintBinary<byte>(SPI0_INTCTRL);
  Serial.print("SPI0_INTFLAGS: ");
  serialPrintBinary<byte>(SPI0_INTFLAGS);
  Serial.print("SPI0_DATA: ");
  serialPrintBinary<byte>(SPI0_DATA);

  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(SS, INPUT); 
}

void loop() {
  if (process_it) { 
    if(isStringMessage){
      Serial.print((char *)readBuffer);
    }
    else {
      Serial.println("SPI: packet received");
      readBuffer [pos] = 0;
      Serial.println("SPI: Received data:");
      for(int i=0; i < messageSize ; i++) {
        Serial.println(readBuffer[i],DEC);
      }  
    }
    process_it = false;
    pos = 0;   
  }  // end of flag set
}

// SPI interrupt routine
ISR(SPI0_INT_vect) {
  controlFlag = true;
  byte c = SPI0.DATA;
  /* Echo Reply */
  SPI0.DATA = c;
  //Serial.println((char)c);
  //interruptCounter ++;
  // add to readBufferf if room
  if (pos < sizeof(readBuffer)) {
     readBuffer [pos++] = c;
     if ((char)readBuffer[pos-1] == '\n' and (char)readBuffer[pos-2] == '\r' and process_it == false) {
       process_it = true;
       readBuffer[pos] = '\0';
       messageSize = pos;
       pos = 0;
     }  
  }
  SPI0.INTFLAGS = SPI_IF_bm; /* Clear the Interrupt flag by writing 1 */
}

#endif

#if 1
Slave Arduino Code:

//SPI SLAVE (ARDUINO)

//SPI COMMUNICATION BETWEEN TWO ARDUINO 

//CIRCUIT DIGEST

//Pramoth.T


#include<SPI.h>

#define LEDpin 7

#define buttonpin 2

volatile boolean received;

volatile byte Slavereceived,Slavesend;

int buttonvalue;

int x;

void setup()


{

  Serial.begin(115200);

  

  pinMode(buttonpin,INPUT);               // Setting pin 2 as INPUT

  pinMode(LEDpin,OUTPUT);                 // Setting pin 7 as OUTPUT

  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 


  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode

  received = false;


  SPI.attachInterrupt();                  //Interuupt ON is set for SPI commnucation

  

}


ISR (SPI_STC_vect)                        //Inerrrput routine function 

{

  Slavereceived = SPDR;         // Value received from master if store in variable slavereceived

  received = true;                        //Sets received as True 

}


void loop()

{ if(received)                            //Logic to SET LED ON OR OFF depending upon the value recerived from master

   {

      if (Slavereceived==1) 

      {

        digitalWrite(LEDpin,HIGH);         //Sets pin 7 as HIGH LED ON

        Serial.println("Slave LED ON");

      }else

      {

        digitalWrite(LEDpin,LOW);          //Sets pin 7 as LOW LED OFF

        Serial.println("Slave LED OFF");

      }

      

      buttonvalue = digitalRead(buttonpin);  // Reads the status of the pin 2

      

      if (buttonvalue == HIGH)               //Logic to set the value of x to send to master

      {

        x=1;

        

      }else

      {

        x=0;

      }

      

  Slavesend=x;                             

  SPDR = Slavesend;                           //Sends the x value to master via SPDR 

  delay(1000);

}

}

#endif