/**
 * Autosampler
 * 
 * Copyright Jeremy Hise, 2016
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#define BUTTON_PIN 2

#define PUMP_RUN_TIME 30 // Seconds
#define PUMP_RUN_TIME_TEST 10 // seconds

#define SETTINGS_FILENAME "/SETTINGS.INI"

char pump1Time[20];
char pump2Time[20];
char pump3Time[20];
char pump4Time[20];
char pump5Time[20];
char currentTime[20];
int pump_run_time;
int flush_button_state = 0;

int p1, p2, p3, p4, p5;

volatile boolean runPumpTrigger = false;

void setup() {
  Serial.begin(9600);
  RTC.configure(3,4,5,6);
  p1 = p2 = p3 = p4 = p5 = 0;
  pump_run_time = PUMP_RUN_TIME;
  pinMode(PUMP1_PIN, OUTPUT); analogWrite(PUMP1_PIN, LOW);
  pinMode(PUMP2_PIN, OUTPUT); analogWrite(PUMP2_PIN, LOW);
  pinMode(PUMP3_PIN, OUTPUT); analogWrite(PUMP3_PIN, LOW);
  pinMode(PUMP4_PIN, OUTPUT); analogWrite(PUMP4_PIN, LOW);
  pinMode(PUMP5_PIN, OUTPUT); analogWrite(PUMP5_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT);
  
  initializeFromFile();

  /* 
   * This attachment is being removed because it is being fired when the pumps
   * are under load. To fix that, you need to look at how the relays are being
   * connected to the arduino. There is some resistance needed there. A test with
   * 220 Ohm resistance didn't do the work. Someone suggested 1K, which has not been
   * tried. This handler is now being called within the main loop.
   * attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), triggerRunAllPumps, RISING); 
   */
  

}

void loop() {
  time_t ts_now, ts_test;

  Serial.println(RTC.readDateTime());
  
  // Generate a local now timestamp
  setTime(RTC.time_h(), RTC.time_m(), RTC.time_s(), RTC.date_d(), RTC.date_m(), RTC.date_y());
  
  // Create a local timestamp for pump 1
  ts_now = now();

  Serial.print("TN: ");
  Serial.println(ts_now);
  Serial.print("P1: ");
  Serial.println(getTimeStamp(pump1Time));
  Serial.print("P2: ");
  Serial.println(getTimeStamp(pump2Time));
  Serial.print("P3: ");
  Serial.println(getTimeStamp(pump3Time));
  Serial.print("P4: ");
  Serial.println(getTimeStamp(pump4Time));
  Serial.print("P5: ");
  Serial.println(getTimeStamp(pump5Time));

  flush_button_state = digitalRead(BUTTON_PIN);
  
  if(flush_button_state == HIGH) {
    runAllPumps();
  }
  
  if(ts_now >= getTimeStamp(pump1Time) && (p1 == 0)) {
    p1 = 1;
    runPump(1, pump_run_time);
  }
      
  if(ts_now >= getTimeStamp(pump2Time) && (p2 == 0)) {
    p2 = 1;
    runPump(2, pump_run_time);
  }

  if(ts_now >= getTimeStamp(pump3Time) && (p3 == 0)) {
    p3 = 1;
    runPump(3, pump_run_time);
  }

  if(ts_now >= getTimeStamp(pump4Time) && (p4 == 0)) {
    p4 = 1;
    runPump(4, pump_run_time);
  }

  if(ts_now >= getTimeStamp(pump5Time) && (p5 == 0)) {
    p5 = 1;
    runPump(5, pump_run_time);
  }
  
  delay(5000);
}

void triggerRunAllPumps()
{
  runPumpTrigger = true;
}

void runAllPumps()
{
  runPumpTrigger = false;
  
  for(int i = 1; i < 6; i++) {
    runPump(i, PUMP_RUN_TIME_TEST);
  }
  
}

void runPump(int pump_id, int duration)
{
  Serial.print("Running pump ");
  if(pump_id == 1) {
    analogWrite(PUMP1_PIN,255);
    Serial.println(pump_id);
    delay(duration * 1000);
    Serial.println("D");
    analogWrite(PUMP1_PIN, 0);
  } else if (pump_id == 2) {
    analogWrite(PUMP2_PIN,255);
    Serial.println(pump_id);
    delay(duration * 1000);
    Serial.println("D");
    analogWrite(PUMP2_PIN, 0);    
  } else if (pump_id == 3) {
    analogWrite(PUMP3_PIN,255);
    Serial.println(pump_id);
    delay(duration * 1000);
    Serial.println("D");
    analogWrite(PUMP3_PIN, 0);    
  } else if (pump_id == 4) {
    analogWrite(PUMP4_PIN,255);
    Serial.println(pump_id);
    delay(duration * 1000);
    Serial.println("D");
    analogWrite(PUMP4_PIN, 0);    
  } else if (pump_id == 5) {
    analogWrite(PUMP5_PIN,255);
    Serial.println(pump_id);
    delay(duration * 1000);
    Serial.println("D");
    analogWrite(PUMP5_PIN, 0);    
  }
}



long getTimeStamp(char time_string[20]) 
{
  char t_part[2] = {' ',' '};
  int t_hour, t_min, t_sec, t_month, t_day, t_year;
  time_t ts;


  memcpy(t_part, &time_string[0], 2);
  t_hour = atoi(t_part);


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
  }
  
  if(ini.getValue("datetime", "current_time", buffer, bufferLen)) {
    strcpy(currentTime, buffer);
  }
  
  if (ini.getValue("pumps", "pump1_time", buffer, bufferLen)) {
    strcpy(pump1Time, buffer);
   }

   if (ini.getValue("pumps", "pump2_time", buffer, bufferLen)) {
    strcpy(pump2Time, buffer);
   }
   
   if (ini.getValue("pumps", "pump3_time", buffer, bufferLen)) {
    strcpy(pump3Time, buffer);
   }
   
   if (ini.getValue("pumps", "pump4_time", buffer, bufferLen)) {
    strcpy(pump4Time, buffer);
   }
   
   if (ini.getValue("pumps", "pump5_time", buffer, bufferLen)) {
    strcpy(pump5Time, buffer);
   }

  if (ini.getValue("pumps", "pump_duration", buffer, bufferLen)) {
    pump_run_time = strtol(buffer, NULL, 10);
  }
 
  ini.close();

 if(c_set == 0) {
    Serial.println("Don't set datetime");
  } else {
    char t_part[2];
    
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
      sprintf(log, "pump_duration = %d", pump_run_time);
      settingsFile.println(log); 
  
      settingsFile.close();
    } 
  }

  return(1);
}

