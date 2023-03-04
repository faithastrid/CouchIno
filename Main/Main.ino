#include <SoftwareSerial.h>
#include <Sabertooth.h>
//Packet serial
SoftwareSerial SWSerial(NOT_A_PIN, 14 ); // RX on no pin (unused), TX on pin 14 (to S1).
Sabertooth STL(128, SWSerial);
Sabertooth STR(129, SWSerial);
//  Sabertooth STL(128);
//  Sabertooth STR(129);
//STL and STR refer to the different motor controllers

const float SPEED_PERCENT = .5;

int input_c_R = 0;
int input_c_L = 0;
int output_R = 0;
int output_L = 0;


// Connections to make:
//   Arduino (TX->1)use 14 instead!!!!!!  ->  Sabertooth S1
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


/*
 Example sketch for the Logitech F310 gamepad
 */

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include "lf310.h"

USB Usb;
LF310 lf310(&Usb);


void setup() {

          Serial.begin(115200);
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Starting Logitech F310 gamepad");

        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");
                
        // Set this to higher values to enable more debug information
        // minimum 0x00, maximum 0xff, default 0x80
        // UsbDEBUGlvl = 0xff;

        delay(200);

        
  // put your setup code here, to run once:
  // start communication  
  SWSerial.begin(9600);
  STL.setTimeout(100); // this will cause the motor controls to stop all motors if a new input is not received in this timeframe
  STR.setTimeout(100);
  //only works in increments of 100 milliseconds
  //this will stop motors on .10 seconds without new input

  //  ST.setDeadband(20); //this will create a deadspot from -20 to 20
  //this will stop the motor if the incoming input is in the deadspot for one second
  //will help later to fine tune controller


  cli();//stop interrupts

  //set timer4 interrupt at 10Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  
  // set compare match register for 10hz increments
  OCR4A = 12499/1;// = (16*10^6) / (100*64) - 1 (must be <65536)
  
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 64 prescaler
  TCCR4B |= (1 << CS41) | (1 << CS40);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts

}
/*
 * Main loop
*/
void loop() {
  // put your main code here, to run repeatedly:
  //both joysticks go from 0 to 255
  //forward is zero backwards is 255
  //resting is 127
  Usb.Task();
  if (lf310.connected()) {
    input_c_L = int(SPEED_PERCENT * (127 - lf310.lf310Data.Y));
    input_c_R = int(SPEED_PERCENT * (127 - lf310.lf310Data.Rz));
    delay(20);
    //read input from controller and put it into the variable input_c_L and input_c_R
  }
}

ISR(TIMER4_COMPA_vect){//timer 4 interrupts every 50ms
  output_R = SmoothVelocity_R(input_c_R);
  output_L = SmoothVelocity_L(input_c_L);
//    Serial.print("Output R ");
//   Serial.println(output_R);
//    Serial.print("Output L ");
//    Serial.println(output_L);
  STR.motor(1, output_R);
  STR.motor(2, output_R);
  STL.motor(1, output_L);
  STL.motor(2, output_L);

}

/*
 * Ramp function
 */
 
int CurrVelocity_R [6] = {0,0,0,0,0,0};
int CurrAcceleration_R [6] = {0,0,0,0,0,0};
int CurrJerk_R [6] = {0,0,0,0,0,0};

int ptVelocity_R = 0;
int ptAcceleration_R = 0;
int ptJerk_R = 0;

int MeanVelocity_R = 0;
int MeanAcceleration_R = 0;
int MeanJerk_R = 0;

int CurrVelocity_L [6] = {0,0,0,0,0,0};
int CurrAcceleration_L [6] = {0,0,0,0,0,0};
int CurrJerk_L [6] = {0,0,0,0,0,0};

int ptVelocity_L = 0;
int ptAcceleration_L = 0;
int ptJerk_L = 0;

int MeanVelocity_L = 0;
int MeanAcceleration_L = 0;
int MeanJerk_L = 0;

int SmoothVelocity_R(int Velocity_new_R){
  MeanVelocity_R = MeanVelocity_R + Velocity_new_R - CurrVelocity_R[ptVelocity_R];
  CurrVelocity_R[ptVelocity_R] = Velocity_new_R;
  ptVelocity_R = (ptVelocity_R + 1) % 6;
  return SmoothAcceleration_R(MeanVelocity_R / 6);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_R(int Acceleration_new_R){
  MeanAcceleration_R = MeanAcceleration_R + Acceleration_new_R - CurrAcceleration_R[ptAcceleration_R];
  CurrAcceleration_R[ptAcceleration_R] = Acceleration_new_R;
  ptAcceleration_R = (ptAcceleration_R + 1) % 6;
  return SmoothJerk_R(MeanAcceleration_R / 6);
}

int SmoothJerk_R(int Jerk_new_R){
  MeanJerk_R = MeanJerk_R + Jerk_new_R - CurrJerk_R[ptJerk_R];
  CurrJerk_R[ptJerk_R] = Jerk_new_R;
  ptJerk_R = (ptJerk_R + 1) % 6;
  return MeanJerk_R / 6;
}

int SmoothVelocity_L(int Velocity_new_L){
  MeanVelocity_L = MeanVelocity_L + Velocity_new_L - CurrVelocity_L[ptVelocity_L];
  CurrVelocity_L[ptVelocity_L] = Velocity_new_L;
  ptVelocity_L = (ptVelocity_L + 1) % 6;
  return SmoothAcceleration_L(MeanVelocity_L / 6);
}//here we are doing the first pass of smoothing the instant accerlation by taking a moving average
//all three functions do the same thing, they just create a more smooth curve each time
//they take the mean of the last 6 values and print that to the next function
//this way accerlation is smoother

int SmoothAcceleration_L(int Acceleration_new_L){
  MeanAcceleration_L = MeanAcceleration_L + Acceleration_new_L - CurrAcceleration_L[ptAcceleration_L];
  CurrAcceleration_L[ptAcceleration_L] = Acceleration_new_L;
  ptAcceleration_L = (ptAcceleration_L + 1) % 6;
  return SmoothJerk_L(MeanAcceleration_L / 6);
}

int SmoothJerk_L(int Jerk_new_L){
  MeanJerk_L = MeanJerk_L + Jerk_new_L - CurrJerk_L[ptJerk_L];
  CurrJerk_L[ptJerk_L] = Jerk_new_L;
  ptJerk_L = (ptJerk_L + 1) % 6;
  return MeanJerk_L / 6;
}
