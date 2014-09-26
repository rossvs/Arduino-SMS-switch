/*
 Arduino GSM-switch example code 
 
 Switch on/off radio controlled controlled outlets 
 
 Author: Vladislav Ross, 2014
 Contact: vladislav.ross@gmail.com
 
 You need to download:
  1. rc-switch https://code.google.com/p/rc-switch/
  2. GSMSHIELD http://www.gsmlib.org/
 
 For GSMSHIELD:
  * To change pins for Software Serial, use the two lines in GSM.cpp.
  * If you are using Mega, uncomment "#define MEGA" line in HWSerial.h
  * You can enable debug messages on serial port by defining DEBUG_ON

*/

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
#include "call.h"

#include <RCSwitch.h>

// 433MHz transmitter pin
const byte RCpin = 12;

char groupAddress[6] = "11111";

char smsLetters[] = "ABC";
char* deviceAddresses[] = {
  "10000", //A
  "01000", //B
  "00100"  //C
};

char adminNumber[] = "+74991356235"; //your phone number

CallGSM call;
SMSGSM sms;

RCSwitch mySwitch = RCSwitch();

char number[20];
byte stat=0;
char smsText[11];
byte position;
int deviceLetterIdx = -1;
byte i,j;

void setup() 
{
 gsm.begin(9600);
 delay(10000);
 for(i = 1; i <= 21; i++)
 {
   sms.DeleteSMS(i);
 }
 
 mySwitch.enableTransmit(RCpin);
};

void loop() 
{  
  //hang up all incoming calls
  /*stat=call.CallStatus();
  if(stat==CALL_INCOM_VOICE)
  {
    call.HangUp();    
  }*/
  
  position = sms.IsSMSPresent(SMS_UNREAD); //get new SMS
  if (position) 
  {
    sms.GetSMS(position, number, smsText, 10);
    sms.DeleteSMS(position);
    
    if(strcmp(number, adminNumber) == 0) //accept SMS only from defined number
    {
      for (i = 0; i < sizeof(smsText) - 1; i++)
      {
        if(deviceLetterIdx != -1)
        {
          //got letter, now expecting 0 or 1
          if(smsText[i] == '0')
          {
            mySwitch.switchOff(groupAddress, deviceAddresses[deviceLetterIdx]);
            delay(500);
          }
          if(smsText[i] == '1')
          {
            mySwitch.switchOn(groupAddress, deviceAddresses[deviceLetterIdx]);
            delay(500);
          }
          deviceLetterIdx = -1;
        }
        else
        {
          //waiting for letter A,B,C...
          for(j = 0; j < sizeof(smsLetters) - 1; j++)
          {
            if(smsLetters[j] == smsText[i])
            {
              deviceLetterIdx = j;
              break;
            }
          }
        }
      }
    }
  }  
  
  delay(10000);
};
