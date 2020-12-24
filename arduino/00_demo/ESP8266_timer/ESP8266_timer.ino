#undef BUILTIN_LED

#define BUILTIN_LED D0

volatile int toggle;

void inline handler (void){
  toggle = (toggle == 1) ? 0 : 1;
  digitalWrite(BUILTIN_LED, toggle);
  timer0_write(ESP.getCycleCount() + 40000);
}
 
void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(handler);
  timer0_write(ESP.getCycleCount() + 40000);
  interrupts();
}

void loop() {
  Serial.println("ceva");
  delay(1000);
}
