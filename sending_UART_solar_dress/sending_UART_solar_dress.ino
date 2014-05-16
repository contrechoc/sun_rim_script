/*
* 
 * testing script
 * running on a atmega328, using Jeelib.h
 * Data coming from UART (Raspberry) are sent with a RF12 transceiver
 * contrechoc 2013
 *
 *
 */


#include <JeeLib.h>

char inputString[] = {
  '#','*', B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111
, B11110000,B00001111,B11110000,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111
} ;         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
unsigned char c = 0;
char buf7[] = {
  '#','*', B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111, 
B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111,B00001111} 
; 


int outputLED = 13;

unsigned char bufferLength = 20;

unsigned char offOn = 0;

void setup(){

  rf12_initialize('R', RF12_433MHZ, 212);
  Serial.begin(9600);

  //LED
  pinMode(outputLED, OUTPUT);//PB1
  digitalWrite(outputLED,HIGH);
  delay(100);
  digitalWrite(outputLED,LOW);

  //send test
  rf12_sendStart(0, inputString, bufferLength , 0);

}

void loop(){

  lookForSerial();



  delay(1);

}

void lookForSerial() {
 
  if ( Serial.available() ){

    Serial.readBytes(buf7, bufferLength) ;

    sendChars();
    Serial.flush();
    c=0;
    //signal the sending
    digitalWrite(outputLED,HIGH);
    delay(300);
    digitalWrite(outputLED,LOW);
     delay(100);
  }
 

}

void sendChars(){

  while ((!rf12_canSend()) )
    rf12_recvDone();
    rf12_sendStart(0, buf7 , bufferLength , 0);

  delay(50);



}


