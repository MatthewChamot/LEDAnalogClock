//  Clock Code  
//  Created by Matthew Chamot 
//  March 2018

// Encoder Variables
#define ENC_A 7
#define ENC_B 8
#define CW 0
#define CCW 180
#define STOP 89
volatile byte state = 0;
volatile byte count = 0;
volatile byte dir = CCW;  // direction
volatile byte ledcount = 0;

// Variable keep track of state and timing
bool reset_mode = true;
bool button_pushed = false;
bool setting_time = false;
int pos = 0;
unsigned long hold_time = 0;

// Timer for PWM for the center LED
IntervalTimer ledTimer;

// Servo Variables
#include <Servo.h>
Servo motor;

// Time Variables
#include <TimeLib.h>

// LED Variables
#define LEDC  12
#define LED1  9 //3
#define LED2  6 //4
#define LED3  4 //5
#define LED4  23 //6
#define LED5  20 //9
#define LED6  32 //10
#define LED7  10 //23
#define LED8  5 //22
#define LED9  3 //21
#define LED10 22 //20
#define LED11 21 //25
#define LED12 25 //32


void setup() {
  
  // Encoder
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENC_A), A_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), B_ISR, CHANGE);
  Serial.begin (115200);
  Serial.println("Start");
  ledTimer.begin(centerPWM,1000);  // every .001 seconds 

  // Servo
  motor.attach(1);
  motor.write(STOP);

  // Time
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);

  // LED test on startup
  int LedTime = 100;
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);
  pinMode(LED10, OUTPUT);
  pinMode(LED11, OUTPUT);
  pinMode(LED12, OUTPUT);
  pinMode(LEDC, OUTPUT);
  digitalWrite(LEDC, HIGH);
  delay(LedTime);
  analogWrite(LED1, 127);
  delay(LedTime);
  analogWrite(LED2, 127);
  delay(LedTime);
  analogWrite(LED3, 127);
  delay(LedTime);
  analogWrite(LED4, 127);
  delay(LedTime);
  analogWrite(LED5, 127);
  delay(LedTime);
  analogWrite(LED6, 127);
  delay(LedTime);
  analogWrite(LED7, 127);
  delay(LedTime);
  analogWrite(LED8, 127);
  delay(LedTime);
  analogWrite(LED9, 127);
  delay(LedTime);
  analogWrite(LED10, 127);
  delay(LedTime);
  analogWrite(LED11, 127);
  delay(LedTime);
  analogWrite(LED12, 127);
  delay(LedTime);
  analogWrite(LED12, 0);
  delay(LedTime);
  analogWrite(LED11, 0);
  delay(LedTime);
  analogWrite(LED10, 0);
  delay(LedTime);
  analogWrite(LED9, 0);
  delay(LedTime);
  analogWrite(LED8, 0);
  delay(LedTime);
  analogWrite(LED7, 0);
  delay(LedTime);
  analogWrite(LED6, 0);
  delay(LedTime);
  analogWrite(LED5, 0);
  delay(LedTime);
  analogWrite(LED4, 0);
  delay(LedTime);
  analogWrite(LED3, 0);
  delay(LedTime);
  analogWrite(LED2, 0);
  delay(LedTime);
  analogWrite(LED1, 0);
  
  // Button
  pinMode(14,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(14), buttonpress, FALLING);

    
  // Reseting Mode
  while(reset_mode) {
    if(button_pushed == true) {
      delay(1000);
      if(digitalRead(14)==LOW) {
        reset_mode = false;
        Serial.println("Exiting Setup");
      }
      else {
        motor.write(CCW);
        Serial.println("Going Back");
      }
      button_pushed = false;
    }
    // Move back an half hour
    if(count<=255 && count>50) {
      motor.write(STOP);
      count = 0;
    }
  }

  // Initial Movement
  setting_time = true;
  pos = hour();
  if(pos < 12) {
    pos = pos + 12;
  }
  else {
    pos = pos - 12;
  }
  pos = pos*2;
  dir = CW;
  lightleds();
  motor.write(dir);

  while(setting_time) {
    if(count==pos) {
      motor.write(STOP);
      lightleds();
      setting_time = false;
    }
  }
  
}

void loop() {
    
  // Read the hour from the RTC and convert to encoder position
  pos = hour();
  if(pos < 12) {
    pos = pos + 12;
  }
  else {
    pos = pos - 12;
  }
  pos = pos*2;

  // Every 24 hours the clock fully reverses to prevent twisting of the LED wiring.
  if(count == 46 && pos == 0) {
    dir = CCW;
    motor.attach(1);
    motor.write(dir);
    setting_time  = true;
    while(setting_time) {
      if(count == 0) {
        motor.write(STOP);
        motor.detach();
        setting_time = false;
      }
      if(count > 200) {
        error();
      }
    }
    dir = CW;
    analogWrite(LED12, 0);
    analogWrite(LED11, 0);
    analogWrite(LED10, 0);
    analogWrite(LED9, 0);
    analogWrite(LED8, 0);
    analogWrite(LED7, 0);
    analogWrite(LED6, 0);
    analogWrite(LED5, 0);
    analogWrite(LED4, 0);
    analogWrite(LED3, 0);
    analogWrite(LED2, 0);
    analogWrite(LED1, 0);
  }

  // Every hour the clock hand moves 1/12 of a full circle
  if(count < pos) {
    motor.attach(1);
    motor.write(60);
    setting_time = true;
    while(setting_time) {
      if(count==pos) {
        motor.write(STOP);
        motor.detach();
        setting_time = false;
      }
    }  
  }
  
  // If the clock hand has rotated more than 2 revolutions, something is wrong
  if(count > 46) {
    error();
  }

  // Check the time every 10 seconds and light the LEDs to display minutes
  Serial.println(count);
  lightleds();
  delay(10000);

}



// ISR for encoder, switch A
// Encoder alternates ABAB for one direction and BABA for the other.
void A_ISR() {
  Serial.print("A ");
  if (dir == CW)
  {
    if (state == 1) state = 2;
    if (state == 3)
    {
      state = 0;
      count++;  
    }
  }

  else if (dir == CCW)
  {
    if (state == 0) state = 1;
    if (state == 2) state = 3;
  }
}

// ISR for encoder, switch B
void B_ISR() {
  Serial.print("B ");
  if (dir == CW)
  {
    if (state == 0) state = 1;
    if (state == 2) state = 3;
  }

  else if (dir == CCW)
  {
    if(state == 1) state = 2;
    if (state == 3)
    {
      state = 0;
      count--;  
    }
  }
}

// Reset button
void buttonpress() {
  //motor.write(STOP);
  button_pushed = true;
}

// Real Time Clock functionality
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}


// Light the LEDs to display the number of minutes past the hour
void lightleds() {
  int m = minute();
  int maxPWM = 32;
  int value;
  if(m%5 == 4)
    value = maxPWM/2;
  else if(m%5 == 3)
    value = maxPWM/4;
  else if(m%5 == 2)
    value = maxPWM/8;
  else if(m%5 == 1)
    value = maxPWM/16;
  else
    value = 0;
     

  
  if(m>=1 && m <= 4)
    analogWrite(LED1, value*.8);
  else if(m>=5 && m <= 9) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, value);
  }
  else if(m>=10 && m <= 14) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, value);
  }
  else if(m>=15 && m <= 19) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, value);
  }
  else if(m>=20 && m <= 24) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, value);
  }
   
  else if(m>=25 && m <= 29) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, value*.6);
  }
  else if(m>=30 && m <= 34) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, value*.8);
  }
  else if(m>=35 && m <= 39) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, maxPWM*.8);
    analogWrite(LED8, value*.8);
  }
  else if(m>=40 && m <= 44) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, maxPWM*.8);
    analogWrite(LED8, maxPWM*.8);
    analogWrite(LED9, value*.8);
  }
  else if(m>=45 && m <= 49) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, maxPWM*.8);
    analogWrite(LED8, maxPWM*.8);
    analogWrite(LED9, maxPWM*.8);
    analogWrite(LED10, value*.8);
  }
  else if(m>=50 && m <= 54) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, maxPWM*.8);
    analogWrite(LED8, maxPWM*.8);
    analogWrite(LED9, maxPWM*.8);
    analogWrite(LED10, maxPWM*.8);
    analogWrite(LED11, value*.8);
  }
  else if(m>=55 && m <= 59) {
    analogWrite(LED1, maxPWM*.8);
    analogWrite(LED2, maxPWM);
    analogWrite(LED3, maxPWM);
    analogWrite(LED4, maxPWM);
    analogWrite(LED5, maxPWM);
    analogWrite(LED6, maxPWM*.6);
    analogWrite(LED7, maxPWM*.8);
    analogWrite(LED8, maxPWM*.8);
    analogWrite(LED9, maxPWM*.8);
    analogWrite(LED10, maxPWM*.8);
    analogWrite(LED11, maxPWM*.8);
    analogWrite(LED12, value*.8);
  }
  else {
    analogWrite(LED1, 0);
    analogWrite(LED2, 0);
    analogWrite(LED3, 0);
    analogWrite(LED4, 0);
    analogWrite(LED5, 0);
    analogWrite(LED6, 0);
    analogWrite(LED7, 0);
    analogWrite(LED8, 0);
    analogWrite(LED9, 0);
    analogWrite(LED10, 0);
    analogWrite(LED11, 0);
    analogWrite(LED12, 0);
  }
}

// Ran out of PWM pins so center LED uses a seperate timer for PWM
void centerPWM() {
  if(ledcount < 4) {
    digitalWrite(LEDC, LOW);
    ledcount++;
  }
  else {
    digitalWrite(LEDC, HIGH);
    ledcount = 0;
  }
}


// If something fails everything should shut to prenvent the wires from becoming too twisted.
void error() {
  analogWrite(LED12, 0);
  analogWrite(LED11, 0);
  analogWrite(LED10, 0);
  analogWrite(LED9, 0);
  analogWrite(LED8, 0);
  analogWrite(LED7, 0);
  analogWrite(LED6, 0);
  analogWrite(LED5, 0);
  analogWrite(LED4, 0);
  analogWrite(LED3, 0);
  analogWrite(LED2, 0);
  analogWrite(LED1, 0);
  motor.detach();
  count = 0;
  while(true) {
    digitalWrite(LEDC, HIGH);
    delay(1000);
    digitalWrite(LEDC, LOW);
    delay(1000);
  }
}


