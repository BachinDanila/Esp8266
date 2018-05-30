#include <SPI.h>
#include <Wire.h>
#include <PS2Keyboard.h>

volatile boolean process_it;                                   //Flag for checking if the data is recieved from Master i.e. ESP8266
byte a; 
String str = "";
int i = 0,len = 0;
#define DataPin 2
#define IRQpin 3

PS2Keyboard keyboard;

//Byte to store the processed data
void setup(void)
{
    delay(1000);
    Serial.begin(115200);
    keyboard.begin(DataPin, IRQpin);//Set UART baug rate to 115200
    SPCR |= bit(SPE);                                         //Configure ATMEGA328P/Arduino in slave mode
    pinMode(MISO, OUTPUT);                                    //Configure MISO as output, SlaveOut
    process_it = false;                                       //Initialize flag to FALSE
    SPI.attachInterrupt();                                    //Enable interrupts for SPI--> You can do that like this too /*SPCR |= bit (SPIE)*/
}
 
// SPI interrupt routine
ISR(SPI_STC_vect)
{
    byte c = SPDR;                                           //Grab the data byte from the SPI Data Register (SPDR)
    a = c;
    if(a != 2 && a != 0){
          str += char(a);
    }
    if (keyboard.available()) {
      char n = keyboard.read();//Put the byte into a temporary variable for processing
      SPDR = n; //process the data byte and put it back into the SPDR for the Master to read it
    }
    else{
      SPDR = -1;
    }
    process_it = true;  //Set the Flag as TRUE
}
 
void loop(void)
{
    if (process_it)                                         //Check if the data has been processed
    {                 //UART - Notify if recived a byte from master
        process_it = false; 
        Serial.println(str);
        //Set the Flag to False
    }
}



