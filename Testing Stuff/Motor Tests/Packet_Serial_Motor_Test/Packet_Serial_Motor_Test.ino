/*
 * This code allows u to specify a speed on the serial monitor and it will output it to all motors one by one
 */



#include <SoftwareSerial.h>
#include <Sabertooth.h>
//Packet serial
SoftwareSerial SWSerial(NOT_A_PIN, 14 ); // RX on no pin (unused), TX on pin 14 (to S1).
Sabertooth STL(128, SWSerial);
Sabertooth STR(129, SWSerial);
//STL and STR refer to the different motor controllers

//later, we can comdense both motor controllers to one class such as
//Sabertooth ST [2] = { Sabertooth(128), Sabertooth(129) };


long int motorSpeed = 0;
// Connections to make:
//   Arduino TX->1  ->  Sabertooth S1
//   Arduino GND    ->  Sabertooth 0V
//   Arduino VIN    ->  Sabertooth 5V (OPTIONAL, if you want the Sabertooth to power the Arduino)
// baud rate 9600
//dip switches for address 128
// 1 on
// 2 off
// 3 on
// 4 off
// 5 on
// 6 off

//dip switches for address 129
// 1 off
// 2 off
// 3 on
// 4 off
// 5 on
// 6 on
void setup() {
  // put your setup code here, to run once:
  SWSerial.begin(9600);
  SabertoothTXPinSerial.begin(9600);// start communication
  Serial.println("Enter motor speed(-127 to 127)..."); // ask user input
  STL.setTimeout(10000); // this will cause the motor controls to stop all motors if a new input is not received in this timeframe
  STR.setTimeout(10000);
  //only works in increments of 100 milliseconds
  //this will stop motors on 10 seconds without new input

//  ST.setDeadband(20); //this will create a deadspot from -20 to 20
  //this will stop the motor if the incoming input is in the deadspot for one second
  //will help later to fine tune controller

    STL.setRamping(14); //this makes sure the motors dont burn themselves out when changing speed suddenly
    STR.setRamping(14);


   // STL.setMinVoltage(30);//will turn off motors if the voltage gets too low, such as low battery
   // STR.setMinVoltage(30);//Value = (desired volts-6) x 5 
 
    //ST.setMaxVoltage(71);//this will stop regenerative braking if they start producing more power than the battery can accept at a time
    //ST.setMaxVoltage(71);//Value = Desired Volts*5.12
    //according to the documentation this should not matter since we are using a battery, we can leave at default
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){ //receive user input
    motorSpeed = Serial.parseInt();
    Serial.print("Motor Speed: ");
    Serial.println(motorSpeed);
      
    if (motorSpeed >= -127 && motorSpeed <= 127){
      STL.motor(1, motorSpeed); //turn on motor 1
      delay(5000); //wait 5 sec
      STL.motor(1, 0); // turn off motor 1
      STL.motor(2, motorSpeed); // turn on motor 2
      delay(5000);//wait 5 sec
      STL.motor(2, 0); // turn off motor 2

      
      STR.motor(1, motorSpeed); //turn on motor 1
      delay(5000); //wait 5 sec
      STR.motor(1, 0); // turn off motor 1
      STR.motor(2, motorSpeed); // turn on motor 2
      delay(5000);//wait 5 sec
      STR.motor(2, 0); // turn off motor 2
    }
  }
}
