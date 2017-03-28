 /* HOH S.A.S. */
 /* 23 Mar 2017 */
 /* Arduino Pro Mini */
 /* Simple ultrafiltering plant automatization */
 
 /* LCD RS pin to digital pin 8
 *  LCD Enable pin to digital pin 9
 *  LCD D4 pin to digital pin 4
 *  LCD D5 pin to digital pin 5
 *  LCD D6 pin to digital pin 6
 *  LCD D7 pin to digital pin 7
 *  
 *  VeUf - NO - pin 13
 *  Vci  - NC - pin 2
 *  Vcs  - NC - pin 3
 *  Vrt  - NC - pin 11
 *  VsUf - NO - pin 12
 *  
 *  time_input - pot - pin A0 */

#include <EEPROM.h>
#include <LiquidCrystal.h>

unsigned long ff_time = 20000;
unsigned long no_time = 60000;
unsigned long bwt_fill_time = 10000;
unsigned long dr_time = 10000;
unsigned long bw_time = 20000;

//        Active Valves:      VsUf  Vrt  Vcs  Vci  VeUf
const int activeValves[5] = {  12,   11,  3,   2,   13   };

//        Valves counter:         VsUf  Vrt  Vcs  Vci  VeUf
unsigned long valveCounter[5] = {  0,    0,   0,   0,   0  };

char* nameValves[6] = {"VsUf", "Vrt", "Vcs", "Vci", "VeUf"};

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  Serial.println("**** Valor inicial ****");
  for(int j=0; j<5; j++){
    pinMode(activeValves[j], OUTPUT);
    valveCounter[j] = EEPROMReadlong(j*4);
    Serial.print(nameValves[j]);
    Serial.print(": ");
    Serial.println(valveCounter[j]);
  } 
  lcd.begin(16, 2);
  Serial.println("***********************");
  delay(500);
}

void loop() {
  steps();
}

void steps(){
  for (int steps=0; steps<5; steps++){
    valve_on(steps);
  }
}

void valve_on(int steps){
  switch (steps){
    case 0: 
      activate(0x05);
      displayStep(0);
      break;
    case 1:
      activate(0x00);
      displayStep(1);
      break;
    case 2:
      activate(0x01);
      displayStep(2);
      break;
    case 3:
      activate(0x19);
      displayStep(3);
      break; 
    case 4:
      activate(0x1B);
      displayStep(4);
      break;  
  }
}

void activate(byte valves){
  for (int i=0; i<5; i++){
    if (bitRead(valves, i) == 0){
      digitalWrite(activeValves[i], LOW);
    }
    else {
      digitalWrite(activeValves[i], HIGH);
      valveCounter[i]++;
    }
    EEPROMWritelong(i*4, valveCounter[i]);
    Serial.println(valveCounter[i]);
  }
  Serial.println("***********************");
}

void displayStep(int steps){
  switch (steps){
    case 0: 
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Enjuague");
      lcd.setCursor(2, 1);
      lcd.print(ff_time/1000);
      lcd.setCursor(4, 1);
      lcd.print("seg");
      timer(ff_time);
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Operacion Normal");
      lcd.setCursor(2, 1);
      lcd.print(no_time/1000);
      lcd.setCursor(4, 1);
      lcd.print("seg");
      timer(no_time);
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Llenado Tanque R");
      lcd.setCursor(2, 1);
      lcd.print(bwt_fill_time/1000);
      lcd.setCursor(4, 1);
      lcd.print("seg");
      timer(bwt_fill_time);
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Drenado");
      lcd.setCursor(2, 1);
      lcd.print(dr_time/1000);
      lcd.setCursor(4, 1);
      lcd.print("seg");
      timer(dr_time);
      break; 
    case 4:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Retrolavado");
      lcd.setCursor(2, 1);
      lcd.print(bw_time/1000);
      lcd.setCursor(4, 1);
      lcd.print("seg");
      timer(bw_time);
      break;  
  }
}

void timer(unsigned long secs){
  unsigned long timing = millis();
  while (millis() - timing < secs){
    lcd.setCursor(9, 1);
    if(secs - (millis() - timing) > 9000){
      lcd.print((secs/1000) - ((millis()-timing)/1000));
    }
    else{
      lcd.print("0");
      lcd.setCursor(10, 1);
      lcd.print((secs/1000) - ((millis()-timing)/1000));
    }
    lcd.setCursor(11, 1);
    lcd.print("seg");
    delay(995);
  }
}

void EEPROMWritelong(int address, long value){
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address){
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
