#include <SPI.h>
#include <LoRa.h>
#include<LiquidCrystal_I2C.h>


int c=16; //no.of columns
int r=2; //no.of rows
LiquidCrystal_I2C lcd(0x27, c, r);
const int r1 = 32; // Pin connected to the relay
const int r2 = 33;
#define SS 5          //ss pin gpio d5
#define RST 14       //reset pin gpio d14 
#define DIO0 4 
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(r1, OUTPUT);
  pinMode(r2,OUTPUT);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed. Check your connections.");
    while (1);
  }
  
  Serial.println("LoRa Initialized");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
 
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      Serial.print(receivedChar);
  lcd.setCursor(0,0);
  lcd.print(receivedChar);
      lcd.setCursor(0,1);
      lcd.print(LoRa.packetRssi());
      // Check if received character is '1' (turn on relay) or '0' (turn off relay)
      if (receivedChar == '1') {
        digitalWrite(r1, HIGH); // Turn on the relay
        Serial.println("' - Turning relay ON");
      } else if (receivedChar == '2') {
        digitalWrite(r1, LOW); // Turn off the relay
        Serial.println("' - Turning relay OFF");
      }
      else if(receivedChar == '3'){
     digitalWrite(r2,HIGH);
     Serial.println("relay 2 on");
      }
      else if(receivedChar == '4'){
        digitalWrite(r2,LOW);
        Serial.println("relay2 off");
      }
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
