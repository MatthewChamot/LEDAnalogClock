/* LED Blink, Teensyduino Tutorial #1
   http://www.pjrc.com/teensy/tutorial.html
 
   This example code is in the public domain.
*/

// Teensy 2.0 has the LED on pin 11
// Teensy++ 2.0 has the LED on pin 6
// Teensy 3.x / Teensy LC have the LED on pin 13
const int ledPin = 10;

// the setup() method runs once, when the sketch starts

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  analogWrite(ledPin, 255);   // set the LED on
  delay(1000);                  // wait for a second
  analogWrite(ledPin, 127);    // set the LED off
  delay(1000);                  // wait for a second
  analogWrite(ledPin, 63);    // set the LED off
  delay(1000);                  // wait for a second
  analogWrite(ledPin, 31);    // set the LED off
  delay(1000);                  // wait for a second
  analogWrite(ledPin, 15);    // set the LED off
  delay(1000);                  // wait for a second
  analogWrite(ledPin, 0);    // set the LED off
  delay(1000);                  // wait for a second
}

