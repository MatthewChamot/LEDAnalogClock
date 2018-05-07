/* LED Blink, Teensyduino Tutorial #1
   http://www.pjrc.com/teensy/tutorial.html
 
   This example code is in the public domain.
*/

// Teensy 2.0 has the LED on pin 11
// Teensy++ 2.0 has the LED on pin 6
// Teensy 3.x / Teensy LC have the LED on pin 13
const int ledPin = 13;
#include <Servo.h>
Servo motor;
// the setup() method runs once, when the sketch starts

void setup() {
  pinMode(ledPin, OUTPUT);
  motor.attach(3);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  motor.write(180);
  delay(20000);
  motor.write(90);
  delay(2000);
}

