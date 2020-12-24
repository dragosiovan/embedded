void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial.println ("Initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available () > 0) {
    switch (Serial.read ()) {
      case 'H':
      case 'h':
        Serial.println ("Hello");
        break;
      case 'W':
      case 'w':
        Serial.println ("World");
        break;
      default:
        break; 
    }
  }
}
