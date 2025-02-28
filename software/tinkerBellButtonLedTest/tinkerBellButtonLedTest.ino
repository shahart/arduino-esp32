
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(D5, OUTPUT);
  pinMode(D7, INPUT);

    Serial.begin(9600);

}

// the loop function runs over and over again forever
void loop() {
  int buttonState;
  int accumulator = 0;
  for(int i = 0; i<50; i++)
  {
    buttonState = digitalRead(D7);
        Serial.println(accumulator);
  delay(1);  // delay in between reads for stability
    accumulator = accumulator+buttonState;
  }


  if(accumulator > 5)
  {
    digitalWrite(D5, HIGH);  // turn the LED on (HIGH is the voltage level)
   delay(1000);
    }
    else
    {
    digitalWrite(D5, LOW);  // turn the LED on (HIGH is the voltage level)
   delay(1000);
    }
    accumulator = 0;
                    // wait for a second
  
  
 }
