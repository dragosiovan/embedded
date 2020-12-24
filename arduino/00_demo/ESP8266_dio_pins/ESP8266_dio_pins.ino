int i = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  delay (10);

}

int value = 0;
byte incomingByte = 0;


void loop() {
  // put your main code here, to run repeatedly:

  char buf[10];
  while (!Serial.available());
  Serial.readBytesUntil ('\n', buf, 4);
  int incom = atoi (buf);
  Serial.println (incom);

  pinMode (incom, OUTPUT);
  digitalWrite (incom, HIGH);
  delay (1000);
  digitalWrite (incom, LOW);
  delay (1000);
  


//  if (Serial.available() > 0) 
//  {
//    // read the incoming byte:
//    incomingByte = Serial.read();
//
//
//    if ((incomingByte == 10) || (incomingByte == 13))
//        {
//          Serial.print ("I received: ");
//          Serial.println (value, DEC);
//          pinMode (value, OUTPUT);
//          digitalWrite (value, HIGH);
//          delay (1000);
//          digitalWrite (value, LOW);
//          delay (1000);
//          
//          value = 0;
//        }
//    else
//        {
//        value = value * 10 + (incomingByte - 0x30);
//        }
//    
//  }


}
