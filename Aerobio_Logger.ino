//Libraries: 
  #include <SPI.h> // SD Card
  #include <SD.h> // SD Card
  #include <SoftwareSerial.h> // GPS
  #include <TinyGPS.h> // GPS
  #include <Adafruit_Sensor.h> // Accel. & Mag.
  #include <Adafruit_LSM303_U.h> // Accel. & Mag.
  #include <Adafruit_MPL3115A2.h> // Altimeter
  #include <Adafruit_HTU21DF.h> // Rel. humidity

// declarations:
  TinyGPS gps; // GPS
  SoftwareSerial ss(3, 2); // GPS
  File dataFile; // for SD card
  // Adafruit_LSM303 lsm; //Accel + Magn
  Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345); //Accel + Magn
  Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
  Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2(); // Altimeter  
  Adafruit_HTU21DF htu = Adafruit_HTU21DF(); //Rel humidity

//constants:
  const int CS = 10;   // uSD Chip Select 
  const int pd1 = A6;  // Vis Sensor (BG PD)
  const int pd2 = A7;  // Vis Sensor (LED PD) - Should be larger
  const int LED = 4; // Vis sensor
  const int analogPinForRV = A1; // Wind sensor 
  const int analogPinForTMP = A0; // Wind sensor

void setup() {
  Serial.begin(57600); 
  Serial.println(F("")); Serial.println(F("Starting Setup"));
  //---------------GPS------------------------------------------
      ss.begin(9600);
      Serial.println(F("completed GPS setup"));
  // --------------ACCEL & MAGN---------------------------------
    //old
      // if (!lsm.begin())  {
      //   Serial.println(F("!! Couldn't find accel sensor!"));
      //   // while (1);
      // }     
      // else { 
      //   Serial.println(F("completed accel setup")); 
      // } 
    
    mag.enableAutoRange(true);   /* Enable auto-gain */

    if(!mag.begin())  {
      Serial.println("!! Couldn't find Magnetometer");
    }  else {
    Serial.println("completed Magnetometer setup");
    }

    if(!accel.begin())  {
      Serial.println("!! Couldn't find Accel");
    }  else {
    Serial.println("completed Accel setup");
    }

  // --------------HUMIDITY------------------------------------
    if (!htu.begin()) {
      Serial.println(F("!! Couldn't find humidity sensor!"));
      // while (1);
    } else { 
    Serial.println(F("completed humidity setup"));
    }
  // --------------ALTIMETER-----------------------------------
    if (!baro.begin()) {
      Serial.println(F("!! Couldnt find Altimeter"));
      // while (1);
    } 
    else { 
    Serial.println(F("completed altimeter setup"));
    }
  // --------------VISIBILITY-----------------------------------
    pinMode(LED, OUTPUT);
    pinMode(pd1, INPUT);
    pinMode(pd2, INPUT);
    Serial.println(F("completed visibility setup"));
  // --------------SD CARD & SM---------------------------------  
    if (!SD.begin(CS)) {   // see if the card is present & can be initialized:       
      Serial.println(F("Card failed, or not present"));
      while (1);
    }
    Serial.println(F("completed sd card setup"));
    
    dataFile = SD.open("data.csv", FILE_WRITE);

     if (dataFile) {  // if the file is available, write to it:
       dataFile.println();
       dataFile.println(F("Sats, Latitude, Longitude, Date, Time, Alt(m), Ang Dir(deg), Speed(kmh), AccelX (m/s2), AccelY, AccelZ, MagX (uT), MagY, MagZ, Pres.(Pa), Alt.(m), Temp1(C), Humidity, Temp2(C), Wind (mph), PD1.(V), PD2")); 
       dataFile.close();

       Serial.println();
       Serial.println(F("Sats, Latitude, Longitude, Date, Time, Alt (m), Ang Dir (deg), Speed(kmh), AccelX (m/s2), AccelY, AccelZ, MagX (uT), MagY, MagZ, Pres.(Pa), Alt.(m), Temp1(C), Humidity, Temp2(C), Wind (mph), PD1.(V), PD2"));
     }
     else {  
       Serial.print(F("Error writing headers to file"));
     }
}

void loop() {
  //---------------GPS-----------------------------------------
    float flat, flon;
    unsigned long age, date, time, chars = 0;
    
    print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    // print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);

    gps.f_get_position(&flat, &flon);
    print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    
    print_date(gps); // date in UT time (7 hours ahead of PST)
    print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2); // alt
    print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2); // angle or direction
    print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2); // speed

  // --------------ACCEL & MAGN--------------------------------
    //old
      // if (!lsm.begin())  {
      //   Serial.print(F("can't find accel sens, "));
      // }   
      // else {
      // lsm.read();
      //   logInt((int)lsm.accelData.x); //column 6
      //   logInt((int)lsm.accelData.y); //column 7 
      //   logInt((int)lsm.accelData.z); //column 8
      //   logInt((int)lsm.magData.x); //column 9
      //   logInt((int)lsm.magData.y); //column 10
      //   logInt((int)lsm.magData.z); //column 11
      // }
    if(!mag.begin())  {
      Serial.println("!! Couldn't find magn");
    }  
    else {
      sensors_event_t event;
      accel.getEvent(&event);
        logFloat(event.acceleration.x); //column 6
        logFloat(event.acceleration.y); //column 7 
        logFloat(event.acceleration.z); //column 8
      mag.getEvent(&event);
        logFloat(event.magnetic.x); //column 9
        logFloat(event.magnetic.y); //column 10
        logFloat(event.magnetic.z); //column 11
    }
   
  // --------------ALTIMETER-----------------------------------
    if (!baro.begin()) {
      Serial.print(F("no Alt sens, "));
    } 
    else {
      float pascals = baro.getPressure();
      float altm = baro.getAltitude();
      float tempC = baro.getTemperature();
      logFloat(pascals);  //column 1 
      logFloat(altm);  //column 2
      logFloat(tempC);  //column 3 
    }
  // --------------HUMIDITY------------------------------------
    if (!htu.begin()) {
      Serial.print(F("no humid sens, "));
    }
    else {
      float humid = (float)htu.readHumidity();
      float humidTemp = (float)htu.readTemperature();
      logFloat(humid); //column 12
      logFloat(humidTemp); //column 13
    }
  // --------------WIND SENSOR---------------------------------
    float windSpeed = windSens();
      logFloat(windSpeed);  //column 4
  // --------------VISIBILITY----------------------------------
    pdOut();
  // --------------NEW LINE------------------------------------
    logNewLine();
  smartdelay(1000);
}


static void print_float(float val, float invalid, int len, int prec) {
  if (val == invalid) {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');

    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    dataFile.print(F(", ")); 
    dataFile.close();
  }
  else {
    static char buff[16];
    dtostrf(val, len, prec, buff); //2nd # = length of the string, 3rd # = values after decimal point
    Serial.print(buff); Serial.print(F(", "));
    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    if (dataFile) { 
      dataFile.print(buff); 
      dataFile.print(F(", ")); 
      dataFile.close();
    }
    else {  
      Serial.print(F("Error writing to file!"));
    }

    // Serial.print(val, prec);
    // int vi = abs((int)val);
    // int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    // flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    // for (int i=flen; i<len; ++i)
    //   Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len) {
  char sz[32];
  if (val == invalid) {
    strcpy(sz, "*******");
    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    dataFile.print(F(", ")); 
    dataFile.close();
  }
  else {
    sprintf(sz, "%ld", val);
    sz[len] = 0;
    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    if (dataFile) { 
      dataFile.print(val); 
      dataFile.print(F(", ")); 
      dataFile.close();  
    }
    else {  
      Serial.print(F("Error writing to file!"));
    }
  }
  // for (int i=strlen(sz); i<len; ++i)
  //   sz[i] = ' ';
  // if (len > 0) 
  //   sz[len-1] = ' ';
    Serial.print(sz); Serial.print(F(", "));
  smartdelay(0);
}

static void print_date(TinyGPS &gps) {
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    
  if (age == TinyGPS::GPS_INVALID_AGE) {
    Serial.print(F("********** ******** "));
    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    dataFile.print(F(", , ")); 
    dataFile.close();
  }
  else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d, %02d:%02d:%02d",
        month, day, year, hour, minute, second);
    Serial.print(sz); 
    dataFile = SD.open(F("data.csv"), FILE_WRITE);
    if (dataFile) { 
      dataFile.print(sz); 
      dataFile.print(F(", ")); 
      dataFile.close();  
    }
    else {  
      Serial.print(F("Error writing to file!"));
    }
  }
  // print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void logNewLine() {
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) { 
    dataFile.println(""); // for SD card
    dataFile.close(); 
  }
  else {  
    Serial.print(F("Error writing to file!"));
  }
  Serial.println();  // for Serial Monitor
}

void logFloat(double val) {
  static char buff[16];
  dtostrf(val, 8, 2, buff); //2nd # = length of the string, 3rd # = values after decimal point
  Serial.print(buff); Serial.print(F(", "));
  
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

void logInt(int val) {
  Serial.print(val); Serial.print(F(", "));
  dataFile = SD.open(F("data.csv"), FILE_WRITE);
  if (dataFile) { 
    dataFile.print(val); 
    dataFile.print(F(", ")); 
    dataFile.close();  
  }
  else {  
    Serial.print(F("Error writing to file!"));
  }
}

float windSens() {
  const static float PROGMEM zeroWindAdjustment =  .2; // negative numbers yield smaller wind speeds and vice versa.
    
  int TMP_Therm_ADunits = analogRead(analogPinForTMP);
  int RV_Wind_ADunits = analogRead(analogPinForRV); //was a float earlier
  float RV_Wind_Volts = (RV_Wind_ADunits * 0.0048828125);

  int TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  
  float zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172; 
  float zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;  

  // From a regression: Vraw = V0 + b * WindSpeed ^ c
  // V0 is zero wind at a partic temp. b & c = constants determined by some Excel wrangling with the solver.
  float WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);   
    
  return WindSpeed_MPH;
}

void pdOut() {
  digitalWrite(LED, HIGH);
    float pd1on = analogRead(pd1)* (5.0 / 1023.0); 
    float pd2on = analogRead(pd2)* (5.0 / 1023.0); 
    logFloat(pd1on);
    logFloat(pd2on);
}