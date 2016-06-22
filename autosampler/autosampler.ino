/**
 * Autosampler
 * 
 * Copyright Jeremy Hise, 2016
 * jeremy.hise@gmail.com
 * 
 * 
 * External dependancies
 * IniFile https://github.com/stevemarple/IniFile
 * Time By Michael Margolis
 * DS3234 https://github.com/rodan/ds3234
 * 
 */
#include <Ethernet.h>

#include <Time.h>
#include <TimeLib.h>

#include <SPI.h>
#include <SD.h>
#include <DS3234.h>
#include <IniFile.h>

#define PUMP1_PIN A0
#define PUMP2_PIN A1
#define PUMP3_PIN A2
#define PUMP4_PIN A3
#define PUMP5_PIN A4

#define SETTINGS_FILENAME "/settings.ini"

char pump1Time[20];
char pump2Time[20];
char pump3Time[20];
char pump4Time[20];
char pump5Time[20];

void setup() {
  Serial.begin(9600);
  RTC.configure(3,4,5,6);
  initializeFromFile();
}

void loop() {
  time_t ts_now;
  
  Serial.println(RTC.readDateTime());

  // Generate a local now timestamp
  
  setTime(RTC.time_h(), RTC.time_m(), RTC.time_s(), RTC.date_d(), RTC.date_m(), RTC.date_y());
  
  // Create a local timestamp for pump 1
  ts_now = now();
  Serial.print(" ts_now = ");
  Serial.println(ts_now);
  
    // Compare
    // If time passed and pump1 flag not set
      // Run Pump
      
  // Create a local timestamp for pump 2
    // Compare
    // If time passed and pump2 flag not set
      // Run Pump

  // Create a local timestamp for pump 3
    // Compare
    // If time passed and pump3 flag not set
      // Run Pump

  // Create a local timestamp for pump 4
    // Compare
    // If time passed and pump4 flag not set
      // Run Pump

  // Create a local timestamp for pump 5
    // Compare
    // If time passed and pump5 flag not set
      // Run Pump
  


  delay(5000);


}

long getTimeStamp(char time_string[20]) 
{
  char t_part[2];
  long t_value;

  memcpy(t_part, &time_string[0], 2);
  t_value = strtol(t_part, NULL, 10);
  
  return(t_value);
}

int initializeFromFile()
{
  const size_t bufferLen = 35;
  char buffer[bufferLen];
  char log[bufferLen];
  int c_month,c_day,c_year,c_hour,c_min,c_sec, c_set;
  File settingsFile;
  int cardDetect = 9;
  int chipSelect = 8;
  char fileName[] = "SETTINGS.INI";
  
    
  pinMode(cardDetect, INPUT);

  if (!digitalRead(cardDetect))
  {
    Serial.println(F("No card detected. Waiting for card."));
    while (!digitalRead(cardDetect));
    delay(250); // 'Debounce insertion'
    return(0);
  }

  if(!SD.begin(chipSelect)) {
    Serial.println("Could not initialize SD");
  }
  
  IniFile ini(SETTINGS_FILENAME);
  ini.open();
  Serial.println("Reading settings file");
  
  if (ini.getValue("datetime", "c_set", buffer, bufferLen)) {
    c_set = strtol(buffer, NULL, 10);
  }
  
  if (ini.getValue("pumps", "pump1_time", buffer, bufferLen)) {
    strcpy(pump1Time, buffer);
    Serial.println("Pump 1 Time"); Serial.println(pump1Time);
   } else {
    Serial.println("No pump1 info");
   }
 /*
   if (ini.getValue("pumps", "pump2_time", buffer, bufferLen)) {
    strcpy(pump2Time, buffer);
    Serial.print("Pump 2 Time"); Serial.println(pump2Time);
   }
   if (ini.getValue("pumps", "pump3_time", buffer, bufferLen)) {
    strcpy(pump3Time, buffer);
    Serial.print("Pump 3 Time"); Serial.println(pump3Time);
   }
   if (ini.getValue("pumps", "pump4_time", buffer, bufferLen)) {
    strcpy(pump4Time, buffer);
    Serial.print("Pump 4 Time"); Serial.println(pump4Time);
   }
   if (ini.getValue("pumps", "pump5_time", buffer, bufferLen)) {
    strcpy(pump5Time, buffer);
    Serial.print("Pump 5 Time"); Serial.println(pump5Time);
   }
  */

 

 
    if (ini.getValue("datetime", "c_month", buffer, bufferLen)) {
      c_month = strtol(buffer, NULL, 10);
    }
  
    if (ini.getValue("datetime", "c_day", buffer, bufferLen)) {
      c_day = strtol(buffer, NULL, 10);
    }
    
    if (ini.getValue("datetime", "c_year", buffer, bufferLen)) {
      c_year = strtol(buffer, NULL, 10);
    }
    
    if (ini.getValue("datetime", "c_hour", buffer, bufferLen)) {
      c_hour = strtol(buffer, NULL, 10);
    }
  
    if (ini.getValue("datetime", "c_minute", buffer, bufferLen)) {
      c_min = strtol(buffer, NULL, 10);
    }
    
    if (ini.getValue("datetime", "c_second", buffer, bufferLen)) {
      c_sec = strtol(buffer, NULL, 10);
    }

 if(c_set == 0) {
    Serial.println("Don't set datetime");
  } else {
    sprintf(log, "Setting datetime: %d/%d/%d %d:%d:%d", c_month, c_day, c_year, c_hour, c_min, c_sec);
    Serial.println(log);
    
    // Day month year hour minute second
    RTC.setDateTime(c_day, c_month, c_year, c_hour, c_min, c_sec);
    
  }
  Serial.print("Initial datetime");
  Serial.println(RTC.readDateTime());

  ini.close();

  

  // Rewrite the file, putting c_set to 0
  Serial.print(fileName);
  Serial.print(" ");
  if (SD.exists(fileName))
  {
    Serial.println(F(" exists."));
  }
  else
  {
    Serial.println(F(" doesn't exist. Creating."));
  }

/*
  SD.remove(fileName);

  settingsFile = SD.open(fileName, FILE_WRITE);
  if(settingsFile) {
    settingsFile.println("[datetime]");
    sprintf(log, "c_set = %d", 0);
    settingsFile.println(log);
    sprintf(log, "c_month = %d", c_month);
    settingsFile.println(log);
    sprintf(log, "c_day = %d", c_day);
    settingsFile.println(log);
    sprintf(log, "c_year = %d", c_year);
    settingsFile.println(log);
    sprintf(log, "c_hour = %d", c_hour);
    settingsFile.println(log);
    sprintf(log, "c_minute = %d", c_min);
    settingsFile.println(log);
    sprintf(log, "c_second = %d", c_sec);
    settingsFile.println(log);
    
    settingsFile.println("[pumps]");
    sprintf(log, "pump1_time = %S", pump1Time);
    sprintf(log, "pump2_time = %S", pump2Time);
    sprintf(log, "pump3_time = %S", pump3Time);
    sprintf(log, "pump4_time = %S", pump4Time);
    sprintf(log, "pump5_time = %S", pump5Time);
    

    settingsFile.close();
  } else {
    Serial.println("Could not open settings file for writing.");
    return(0);
  }
  */

  return(1);
}

