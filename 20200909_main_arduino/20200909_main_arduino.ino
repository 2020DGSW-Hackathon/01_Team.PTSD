
#include <SPI.h>
#include <MFRC522.h>

// Keyboard Control
#define PIN_KEY_DATA 14
#define PIN_INTERRUPT 7

// Brightness Data
#define PIN_BRIGHTNESS A1

// NFC Data
#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// IR Data
unsigned long falling_prev = 0;
unsigned long falling_now = 0;

unsigned int diff_time[35];
int receive_index = 0;
int max_flag = 0;
int repeat_flag = 0;
int data_received = 0;

//unsigned char now_data_hex = 0;


// brightness millis
unsigned long long c_millis;
unsigned long long p_millis;


void sendISR(int data_hex);
void sendNFC(byte *buffer, byte bufferSize);

void setup() {
  Serial.begin(9600);

  pinMode(PIN_KEY_DATA, OUTPUT);
  pinMode(PIN_INTERRUPT, OUTPUT);
  
  // NFC Init
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  
  // ISR INIT
  attachInterrupt(0, remocon_ISR, FALLING);

  //
  //Serial.println("BR:0")
}

void loop() {
  
  c_millis = millis();

  if(c_millis - p_millis > 500) {
    p_millis = c_millis;
    int light = analogRead(A1);
    Serial.println("BR:" + String(light));
  }
  
  
  // IR
  if(max_flag == 1) {
    max_flag = 0;
    
    // for stable code that insert oneCount, iStart
    int oneCount = 0;
    int iStart = 16, iEnd = 24;
    long remocon_data[33];
    
    for(int i = 0;i < 34;i++) {
      if(diff_time[i] > 1000 && diff_time[i] < 1500) {
        remocon_data[i - 1] = 0;
      }
      else if(diff_time[i] > 2000 && diff_time[i] < 2500) {
        remocon_data[i - 1] = 1;
        //oneCount++;
        if(++oneCount == 8) {
          iStart = i;
          iEnd = iStart + 8;
        }
      }
      else {
        remocon_data[i - 1] = diff_time[i];
      }
    }
    
    
    int data_hex = 0;
    for(int i=iStart;i<iEnd;i++) {
      if(remocon_data[i] == 1) {
        data_hex |= 0x01 << (iEnd - 1 - i);
      }
    }

    sendISR(data_hex);
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  // Verify if the NUID has been readed
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
  
    //Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    //Serial.println(rfid.PICC_GetTypeName(piccType));
  
    // Check is the PICC of Classic MIFARE type
    if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||  
        piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
        piccType == MFRC522::PICC_TYPE_MIFARE_4K) {
      sendNFC(rfid.uid.uidByte, rfid.uid.size);
    }
    
    // Halt PICC
    rfid.PICC_HaltA();
  
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
  }
  
  
}

void remocon_ISR() {

  falling_now = micros();
  
  if(receive_index > 0) {
    int dt = falling_now - falling_prev;
    diff_time[receive_index - 1] = dt;

    if (dt > 13000 && dt < 15000) {
      receive_index = 1;
    }
    else if (dt > 11000 && dt < 12000) {
      receive_index = 0;
      repeat_flag = 1;
    }
    else if (dt > 20000) { // for stable code, error initialization
      // error initialization
      receive_index = 0;
    }
    
  }
  
  if(++receive_index == 34) {
    receive_index = 0;
    max_flag = 1;
  }

  falling_prev = falling_now;
  
}

void sendISR(int data_hex) {
  Serial.println("IR:" + String(data_hex));
  //now_data_hex = data_hex;
}

void sendNFC(byte *buffer, byte bufferSize) {
  
  int data = (buffer[3] & buffer[2]) + (buffer[1] & buffer[0]);
  
  if(data == 167) {
    writeKeyboard(0xB0);
    delay(100);

    // 3675716
    writeKeyboard(0x31); // 0x3
    delay(100);
    
    writeKeyboard(0xB0);
  }
  
}

void writeKeyboard(unsigned char data) {
  for(char i=0;i<8;i++) {
    if(data & (0x80 >> i)) {
      digitalWrite(PIN_KEY_DATA, HIGH);
    }
    else {
      digitalWrite(PIN_KEY_DATA, LOW);
    }
    
    digitalWrite(PIN_INTERRUPT, HIGH);
    digitalWrite(PIN_INTERRUPT, LOW);
    delayMicroseconds(10);
  }
  //Serial.println("WK:" + String(data) + s);
}
