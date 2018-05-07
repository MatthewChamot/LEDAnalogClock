/* Rotary encoder read example */
#define ENC_A 7
#define ENC_B 8
#define CW 2
#define CCW 1
volatile byte state = 0;
volatile byte count = 0;
volatile byte dir = CW;  // direction

 
void setup()
{
  /* Setup encoder pins as inputs */
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENC_A), A_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), B_ISR, CHANGE);
  Serial.begin (115200);
  Serial.println("Start");
}
 
void loop()
{
  Serial.println(count);
  delay(2000);

}

void A_ISR() {
  //Serial.print("A ");
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

void B_ISR() {
  //Serial.print("B ");
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


 
