const int potPin = A0;
const int ledPin = D4;

int potValue = 0;
int ledValue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  pinMode (potPin, INPUT);
  pinMode (ledPin, OUTPUT);

}

void loop() {
  PWMRANGE
  potValue = analogRead (potPin);

  Serial.print ("Potentiometer value: ");
  Serial.println (potValue);

  analogWrite (ledPin, potValue);
  delay (200);
}
