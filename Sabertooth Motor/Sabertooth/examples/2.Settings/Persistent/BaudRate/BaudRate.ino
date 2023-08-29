#include <SoftwareSerial.h>

// Set Baud Rate Sample for Packet Serial
// Copyright (c) 2012 Dimension Engineering LLC
// See license.txt for license details.

// WARNING: This sample makes changes that will persist between restarts.
// NOTE: The setBaudRate function will only have an effect on V2 controllers (2x12, 2x25 V2, 2x60, SyRen 50).
//       Earlier controllers automatically detect the baud rate you choose in Serial.begin
//       when you call the autobaud function. Autobaud was replaced in V2 controllers for reliability
//       in the event that the Sabertooth lost power.
#include <Sabertooth.h>


SoftwareSerial SWSerial(NOT_A_PIN, 14 ); // RX on no pin (unused), TX on pin 14 (to S1).
Sabertooth STL(128, SWSerial);
Sabertooth STR(129, SWSerial);

void setup()
{
  // This sample will tell the Sabertooth *at 9600 baud* to *switch to 2400 baud*.
  // Keep in mind you must send the command to change the baud rate *at the baud rate
  // the Sabertooth is listening at* (factory default is 9600). After that, if it works,
  // you will be able to communicate using the new baud rate.
  //
  // Options are:
  //   2400
  //   9600
  //   19200
  //   38400
  //   115200 (only supported by some devices such as 2X60 -- check the device's datasheet)
  //
  // WARNING: The Sabertooth remembers this command between restarts.
  // To change your Sabertooth back to its default, you must *be at the baud rate you've
  // set the Sabertooth to*, and then call ST.setBaudRate(9600)
  SWSerial.begin(9600);
  STR.setBaudRate(115200);
  STL.setBaudRate(115200);
  SWSerial.end();
  SWSerial.begin(115200);
}

void loop()
{
  STR.motor(1, 60);
  STR.motor(2, 60);
  STL.motor(1, 60);
  
  STL.motor(2, 60);
}
