/*
Used for controlling LED & recording output from photodiode!
To be eventually calibrated for visibility!
*/
#include <SPI.h> // SD Card
#include <SD.h> // SD Card
File dataFile; // for SD card
const PROGMEM int CS = 10;   // uSD Chip Select 

int pd1 = A6;  // read input on analog pin 2
int pd2 = A7;
int LEDout = 4; //LED output in Digital pin 4

void setup() {
  Serial.begin(9600);  // for serial monitor :)
  pinMode(LEDout, OUTPUT);
  pinMode(pd1, INPUT);
  pinMode(pd2, INPUT);

  if (!SD.begin(CS)) {   // see if the card is present & can be initialized:       
    Serial.println(F("Card failed, or not present"));
  }
  else {
  Serial.println(F("completed sd card setup"));
    dataFile = SD.open("data.csv", FILE_WRITE);
    if (dataFile) { 
      dataFile.println(F(""));
      dataFile.println(F(""));
      dataFile.close(); 
    }
  }
}

void loop() {
  pdOut();
  logNewLine();
}

void pdOut(){
  digitalWrite(LEDout, HIGH);
  delay(1000);
    float pd1on = analogRead(pd1)* (5.0 / 1023.0); 
    float pd2on = analogRead(pd2)* (5.0 / 1023.0); 
    Serial.print("[LED on] pd1: "); Serial.print(pd1on);
    Serial.print(" pd2: "); Serial.print(pd2on);
    logFloat(pd1on);
    logFloat(pd2on);
  delay(1000);

  digitalWrite(LEDout, LOW);
  delay(1000);
    float pd1off = analogRead(pd1)* (5.0 / 1023.0); 
    float pd2off = analogRead(pd2)* (5.0 / 1023.0); 
    Serial.print(" [LED off] pd1: "); Serial.print(pd1off);
    Serial.print(" pd2: "); Serial.print(pd2off);
    logFloat(pd1off);
    logFloat(pd2off);
  delay(1000);
}

void logFloat(double val) {
  static char buff[16];
  dtostrf(val, 8, 2, buff); //2nd # = length of the string, 3rd # = values after decimal point
  // Serial.print(buff); Serial.print(F(", "));
  
  dataFile = SD.open(F("data.csv"), FILE_WRITE);
  if (dataFile) { 
    dataFile.print(buff); 
    dataFile.print(F(", ")); 
    dataFile.close();
  }
  else {  
    Serial.print(F("Error writing to file!"));
  }
}

void logNewLine() {
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) { 
    dataFile.println(F("")); // for SD card
    dataFile.close(); 
  }
  else {  
    Serial.print(F("Error writing to file!"));
  }
  Serial.println();  // for Serial Monitor
}








//unused
  // int potAnalogPin = A0; //pot input pin
  // int potInput = 0; //var to store the value from sensor
  // int potOutput = 0; //var to store the output value

  // potInput = analogRead(potAnalogPin);
  // potOutput = map(potInput, 0, 51, 125, 135); //conversion from 0-1023 to 0-255  
  // Serial.print("potInput: "); Serial.println(potInput);
  // Serial.print("potOutput: "); Serial.println(potOutput);
  // analogWrite(LED, potOutput); 