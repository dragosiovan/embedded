int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps
}

void loop() {

  char ch;
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    ch = Serial.read();

    // say what you got:
    Serial.print(ch);
  }
}
