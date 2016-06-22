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
char currentTime[20];

int p1, p2, p3, p4, p5;


void setup() {
  Serial.begin(9600);
  RTC.configure(3,4,5,6);
  p1 = p2 = p3 = p4 = p5 = 0;
  initializeFromFile();
}

void loop() {
  time_t ts_now, ts_test;

  
  Serial.println(RTC.readDateTime());

  // Generate a local now timestamp
  setTime(RTC.time_h(), RTC.time_m(), RTC.time_s(), RTC.date_d(), RTC.date_m(), RTC.date_y());
  
  // Create a local timestamp for pump 1
  ts_now = now();
  
    // Compare
    // If time passed and pump1 flag not set
    if(ts_now > getTimeStamp(pump1Time) && p1 == 0) {
      // Run Pump
      p1 = 1;
      runPump(1);
    }
      
    if(ts_now > getTimeStamp(pump2Time) && p2 == 0) {
      // Run Pump
      p2 = 1;
      runPump(2);
    }

    if(ts_now > getTimeStamp(pump3Time) && p3 == 0) {
      // Run Pump
      p3 = 1;
      runPump(3);
    }

    if(ts_now > getTimeStamp(pump4Time) && p4 == 0) {
      // Run Pump
      p4 = 1;
      runPump(4);
    }

    if(ts_now > getTimeStamp(pump5Time) && p5 == 0) {
      // Run Pump
      p5 = 1;
      runPump(5);
    }
  


  delay(5000);


}

void runPump(int pump_id)
{
  Serial.print("Running pump ");
  Serial.println(pump_id);
}

void handleDateTimeString(char time_string[20], long& t_hour, long& t_min, long& t_sec, long& t_day, long& t_month, long& t_year)
{

  char t_part[2];
  

  memcpy(t_part, &time_string[0], 2);
  t_hour = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[3], 2);
  t_min = strtol(t_part, NULL, 10);
  
  memcpy(t_part, &time_string[6], 2);
  t_sec = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[9], 2);
  t_day = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[12], 2);
  t_month = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[15], 4);
  t_year = strtol(t_part, NULL, 10);


}

long getTimeStamp(char time_string[20]) 
{
  char t_part[2];
  long t_hour, t_min, t_sec, t_month, t_day, t_year;
  time_t ts;

  memcpy(t_part, &time_string[0], 2);
  t_hour = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[3], 2);
  t_min = strtol(t_part, NULL, 10);
  
  memcpy(t_part, &time_string[6], 2);
  t_sec = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[9], 2);
  t_day = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[12], 2);
  t_month = strtol(t_part, NULL, 10);

  memcpy(t_part, &time_string[15], 4);
  t_year = strtol(t_part, NULL, 10);

  setTime(t_hour, t_min, t_sec, t_day, t_month, t_year);
  ts = now();
  return(ts);
}

int initializeFromFile()
{
  const size_t bufferLen = 40;
  char buffer[bufferLen];
  char log[bufferLen];
  long c_month,c_day,c_year,c_hour,c_min,c_sec, c_set;
  File settingsFile;
  int cardDetect = 9;
  int chipSelect = 8;
  char fileName[] = "SETTINGS.INI";
  
    
  pinMode(cardDetect, INPUT);

  if (!digitalRead(cardDetect))
  {
    Serial.println(F("No card detected. Waiting for card."));
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
    c_set = 1;
  }
  
  if(ini.getValue("datetime", "current_time", buffer, bufferLen)) {
    strcpy(currentTime, buffer);
    Serial.print("Current time: ");
    Serial.println(currentTime);
  }
  
  if (ini.getValue("pumps", "pump1_time", buffer, bufferLen)) {
    strcpy(pump1Time, buffer);
    Serial.print("P1 "); Serial.println(pump1Time);
   }

   if (ini.getValue("pumps", "pump2_time", buffer, bufferLen)) {
    strcpy(pump2Time, buffer);
    Serial.print("P2 "); Serial.println(pump2Time);
   }
   
   if (ini.getValue("pumps", "pump3_time", buffer, bufferLen)) {
    strcpy(pump3Time, buffer);
    Serial.print("P3 "); Serial.println(pump3Time);
   }
   
   if (ini.getValue("pumps", "pump4_time", buffer, bufferLen)) {
    strcpy(pump4Time, buffer);
    Serial.print("P4 "); Serial.println(pump4Time);
   }
   
   if (ini.getValue("pumps", "pump5_time", buffer, bufferLen)) {
    strcpy(pump5Time, buffer);
    Serial.print("P5 "); Serial.println(pump5Time);
   }
 
  ini.close();

 if(c_set == 0) {
    Serial.println("Don't set datetime");
  } else {
   // handleDateTimeString(currentTime, c_hour, c_min, c_sec, c_day, c_month, c_year);
            char t_part[2];

            
          Serial.print(" Handling: ");
          Serial.print(currentTime);
          
            memcpy(t_part, &currentTime[0], 2);
            c_hour = strtol(t_part, NULL, 10);
          
            memcpy(t_part, &currentTime[3], 2);
            c_min = strtol(t_part, NULL, 10);
            
            memcpy(t_part, &currentTime[6], 2);
            c_sec = strtol(t_part, NULL, 10);
          
            memcpy(t_part, &currentTime[9], 2);
            c_day = strtol(t_part, NULL, 10);
          
            memcpy(t_part, &currentTime[12], 2);
            c_month = strtol(t_part, NULL, 10);
          
            memcpy(t_part, &currentTime[15], 4);
            c_year = strtol(t_part, NULL, 10);
    
    // Day month year hour minute second
    RTC.setDateTime(c_day, c_month, c_year, c_hour, c_min, c_sec);


    Serial.println(": Done setting");
  }

  Serial.println(RTC.readDateTime());

  
  // Rewrite the file, putting c_set to 0
  if(c_set == 1) {
    
    SD.remove(fileName);
  
    settingsFile = SD.open(fileName, FILE_WRITE);
    if(settingsFile) {
      settingsFile.println("[datetime]");
      sprintf(log, "c_set = %d", 0);
      settingsFile.println(log);
      sprintf(log, "current_time = %s", currentTime);
      settingsFile.println(log);
      settingsFile.println("[pumps]");
      sprintf(log, "pump1_time = %s", pump1Time);
      settingsFile.println(log);
      sprintf(log, "pump2_time = %s", pump2Time);
      settingsFile.println(log);
      sprintf(log, "pump3_time = %s", pump3Time);
      settingsFile.println(log);
      sprintf(log, "pump4_time = %s", pump4Time);
      settingsFile.println(log);
      sprintf(log, "pump5_time = %s", pump5Time);
      settingsFile.println(log);    
  
      settingsFile.close();
    } 
  }

  return(1);
}

