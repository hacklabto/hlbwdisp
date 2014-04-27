// Master code for HackLab.TO Bandwidth Display
// Receives UDP packets of format AA.AA/BB.BB (decimals anywhere)
// and display them on two 4-digit 7-segment display boards via i2c

// A0  - 
// A1  - 
// A2  - 
// A3  - 
// A4  - I2C SDA
// A5  - I2C SCL
// D0  - Debug RX
// D1  - Debug TX
// D2  - 
// D3  - 
// D4  - 
// D5  - 
// D6  - 
// D7  - 
// D8  - 
// D9  - 
// D10 - Used by Ethernet Shield
// D11 - Used by Ethernet Shield
// D12 - Used by Ethernet Shield
// D13 - Used by Ethernet Shield

#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define DISP_A_BASE_ADDR  1
#define DISP_B_BASE_ADDR  5

byte mac[] = {0xDE, 0xCA, 0xF0, 0xC0, 0xFF, 0xFE };
IPAddress ip(192, 168, 111, 232);
unsigned int listenPort = 1000;

char packetBuffer[16];
char dispBufferA[6];
char dispBufferB[6];
EthernetUDP Udp;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Init...");
  Ethernet.begin(mac, ip);
  //Serial.print("My IP: ");
  //Serial.println(Ethernet.localIP());
  Udp.begin(listenPort);
  Wire.begin();
  // give the user time to look at the address displayed
  delay(2500);
  Serial.println("Ready.");
  
  // empty out the buffers to start
  dispBufferA[0] = NULL;
  dispBufferB[0] = NULL;  
}

void loop(void) {
  updateBuffers();
  updateDisplay(dispBufferA, DISP_A_BASE_ADDR);
  updateDisplay(dispBufferB, DISP_B_BASE_ADDR);
}

void updateBuffers(void) {
  byte pktsplit;
  byte pktend;
  
  int packetSize = Udp.parsePacket();
  if(!packetSize) {
    Serial.print("*");
    return;
  }
  
  Serial.println("Update RX");
  Udp.read(packetBuffer, 16);
  pktsplit = strspn(packetBuffer, "0123456789AbCcdEFLsS.- ");
  pktend = strspn(packetBuffer+pktsplit+1, "0123456789AbBCcdEFLsS.- ");
  if(pktsplit == NULL) return;
  strcpy(dispBufferA, "    ");
  strcpy(dispBufferB, "    ");
  strncpy(dispBufferA, packetBuffer, pktsplit);
  Serial.print("DispA: ");
  Serial.println(dispBufferA);
  strncpy(dispBufferB, packetBuffer+pktsplit+1, pktend);
  Serial.print("DispB: ");
  Serial.println(dispBufferB);
}

void updateDisplay(char *buf, byte base_addr) {
  byte digit = 0;
  byte bufpos = 0;
  while(digit < 4) {
    if(*(buf+bufpos) == '.') bufpos++;
    byte segs = *(buf+bufpos);
    if(digit < 3 && *(buf+bufpos+1) == '.') {
      segs |= 0x80;
    }
    
    Serial.print("Send ");
    Serial.print((char)segs);
    Serial.print(" (");
    Serial.print(segs, DEC);
    Serial.print(") to 0x");
    Serial.println(base_addr+digit, HEX);
    
    Wire.beginTransmission(base_addr + digit);
    Wire.write(segs);
    Wire.endTransmission();
    
    digit++; bufpos++;
  }  
}

