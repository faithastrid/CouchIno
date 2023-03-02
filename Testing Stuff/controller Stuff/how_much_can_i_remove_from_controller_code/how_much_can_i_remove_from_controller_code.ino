// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

/*
 *  this code is all taken from the example that the controller library had
 *  the example works
 *  this example is to show us how much of the code we can strip out and still get the controller working
 */


#include <SPI.h>
#include "lf310.h"

USB Usb;
LF310 lf310(&Usb);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//start talking to logitech controller
  #if !defined(__MIPSEL__)
          while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  delay (200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Usb.Task();
  Serial.print("Left Joystick X: ");
  Serial.println(lf310.lf310Data.X);

  Serial.print("Right Joystick X: ");
  Serial.println(lf310.lf310Data.Z);
  delay(200);

}
