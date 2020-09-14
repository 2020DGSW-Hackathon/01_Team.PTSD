#include <Keyboard.h>

#define PIN_INTERRUPT 2
#define PIN_DATA 18

unsigned long long c_millis;
unsigned long long p_millis;
int data = 0;
char recieve_count = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(PIN_DATA, INPUT);
  pinMode(PIN_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), KeyOUT, RISING);

  Keyboard.begin();

  //delay(5000);
  //Keyboard.write(0x31);
  //Keyboard.write(0x32);
  //Keyboard.write(0x33);
  //Keyboard.write(0x34);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void KeyOUT() {

  c_millis = millis();
  
  if(c_millis - p_millis > 50) {
    p_millis = c_millis;
    recieve_count = 0;
    data = 0;
  }

  //int dr = ;
  data = (data << 1) + digitalRead(PIN_DATA);

  //Serial.print(dr);
  
  
  if(++recieve_count == 8) {
    //Serial.println("key:" + String(data));
    Keyboard.write(data);
    recieve_count = 0;
    data = 0;
  }
  
}
