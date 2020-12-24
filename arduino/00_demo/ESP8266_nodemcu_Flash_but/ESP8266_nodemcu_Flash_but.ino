

#define LED_PIN     D4
#define SW_PIN      D3

void setup() 
{
  Serial.begin (115200);
  delay (10);
  
  pinMode (LED_PIN, OUTPUT);
  pinMode (SW_PIN, INPUT);
}

void loop() 
{
  if (digitalRead (SW_PIN) == HIGH)
  {
    Serial.println ("Switch Unpressed");
  }
  else
  {
    Serial.println ("Switch Pressed");
  }
  

  if (((millis () / 1000) % 2) == 1)
  {
    digitalWrite (LED_PIN, LOW);
  }
  else
  {
    digitalWrite (LED_PIN, HIGH);
  }

  delay (500);

}
