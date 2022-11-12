#include <Arduino.h>
#include <SPI.h>

bool flag =0;
byte s_recv;

const int slaveSelect = 10;

void setup(){
    Serial.begin(9600);
    pinMode(MISO, OUTPUT);
    SPCR = (1<< SPE) | (1 << SPIE);
    sei();
}

ISR (SPI_STC_vect){
    s_recv = SPDR;
    flag =1;
}

void loop(){
    if(flag){
        Serial.println((char(s_recv)));
        delay(500);
        SPDR = 'F';
    }
}