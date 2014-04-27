// Slave/display driver board code for HackLab.TO Bandwidth Display
// Receives bytes over I2C and displays them on an attached 7-segment LED display

// A0  - 
// A1  - 
// A2  - 
// A3  - 
// A4  - I2C SDA
// A5  - I2C SCL
// D0  - Debug RX
// D1  - Debug TX
// D2  - LED Cathode A
// D3  - LED Cathode B
// D4  - LED Cathode C
// D5  - LED Cathode D
// D6  - LED Cathode E
// D7  - LED Cathode F
// D8  - LED Cathode G
// D9  - LED Cathode DP
// D10 - 
// D11 - 
// D12 - 
// D13 - 

#include "Wire.h"

// code expects each segment in order starting at PIN_SEG_BASE
#define PIN_SEG_BASE  2

void setup() {
  byte i, addr;

  // LED pins are outputs
  for(i=2; i<=10; i++) { 
    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);

  // address select pins are pulled-up inputs while we get the address
  // then pull-ups are switched off to save power
  addr = 0;
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  if(digitalRead(A0) == LOW) addr |= 1;
  if(digitalRead(A1) == LOW) addr |= 2;
  if(digitalRead(A2) == LOW) addr |= 4;
  if(digitalRead(A3) == LOW) addr |= 8;
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  addr++;
  
  Wire.begin(addr);
  Wire.onReceive(processByte);
  Serial.print("Address: ");
  Serial.println(addr, DEC);
  
  // start out displaying our address
  digitDisplay(fontDecode(addr));
}

void loop() {
  delay(100);
}

void processByte(int howMany) {
  int data = Wire.read();
  Serial.print(data, DEC);
  Serial.print(" = ");
  Serial.println(fontDecode(data), BIN);
  digitDisplay(fontDecode(data));
}

byte fontDecode(byte in) {
  //  AAAA
  // F    B
  // F    B
  //  GGGG
  // E    C
  // E    C
  //  DDDD  DP
  //                   .GFEDCBA

  byte out;
  byte decimal = 0;
  if(in & 0x80) {
    in = in ^ 0x80;
    decimal = 1;
  }
  
  switch(in) {
  case 0x0:
  case '0':
    out = 0x3F;  // 00111111
    break;
  case 0x1:
  case '1':
  case 'l':
    out = 0x06;  // 00000110
    break;
  case 0x2:
  case '2':
    out = 0x5B;  // 01011011
    break;
  case 0x3:
  case '3':
    out = 0x4F;  // 01001111
    break;
  case 0x4:
  case '4':
    out = 0x66;  // 01100110
    break;
  case 0x5:
  case '5':
  case 'S':
  case 's':
    out = 0x6D;  // 01101101
    break;
  case 0x6:
  case '6':
    out = 0x7D;  // 01111101
    break;
  case 0x7:
  case '7':
    out = 0x07;  // 00000111
    break;
  case 0x8:
  case '8':
    out = 0x7F;  // 01111111
    break;
  case 0x9:
  case '9':
    out = 0x6F;  // 01101111
    break;
  case 0xA:
  case 'a':
  case 'A':
    out = 0x77;  // 01110111
    break;
  case 0xB:
  case 'b':
  case 'B':
    out = 0x7C;  // 01111100
    break;
  case 0xC:
  case 'C':
    out = 0x39;  // 00111001
    break;
  case 'c':
    out = 0x58;  // 01011000
    break;
  case 0xD:
  case 'd':
    out = 0x5E;  // 01011110
    break;
  case 0xE:
  case 'e':
  case 'E':
    out = 0x79;  // 01111001
    break;
  case 0xF:
  case 'f':
  case 'F':
    out = 0x71;  // 01110001
    break;
  case ' ':
    out = 0x00;  // 00000000
    break;
  case '-':
    out = 0x40;  // 01000000
    break;
  case 'L':
    out = 0x38;  // 00111000
    break;
  default:
    out = 0x40;
    break;
  }
  
  if(decimal) out = out | 0x80;
  return out;
}

void digitDisplay(byte data) {
  byte bit;

  for(bit=0; bit<8; bit++) {
    digitalWrite(PIN_SEG_BASE + bit, (data & 1 << bit) == 0 ? LOW : HIGH);
  }
}


