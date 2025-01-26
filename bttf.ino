#include "arduino_secrets.h"
#include <TM1637.h>
#include "thingProperties.h"

// Define connections for TM1637
#define CLK 2
#define DIO 3

TM1637 tm(CLK, DIO);

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

    // Initialize the display
  tm.init();

  // brightness, between 1 and 7
  tm.set(1);
}

DateTime getCurrentDateTime(unsigned long currentTime) {
  DateTime dateTime;
  
  // Convert to hour and minute
  dateTime.hour = (currentTime % 86400L) / 3600;
  dateTime.minute = (currentTime % 3600) / 60;

  // Convert to day and month
  int daysSinceEpoch = currentTime / 86400L;
  dateTime.year = 1970;
  while (daysSinceEpoch >= 365) {
    if ((dateTime.year % 4 == 0 && dateTime.year % 100 != 0) || (dateTime.year % 400 == 0)) {
      if (daysSinceEpoch >= 366) {
        daysSinceEpoch -= 366;
        dateTime.year++;
      }
    } else {
      daysSinceEpoch -= 365;
      dateTime.year++;
    }
  }
  dateTime.month = 0;
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if ((dateTime.year % 4 == 0 && dateTime.year % 100 != 0) || (dateTime.year % 400 == 0)) {
    daysInMonth[1] = 29;
  }
  while (daysSinceEpoch >= daysInMonth[dateTime.month]) {
    daysSinceEpoch -= daysInMonth[dateTime.month];
    dateTime.month++;
  }
  dateTime.day = daysSinceEpoch + 1;

  return dateTime;
}

void loop() {
  ArduinoCloud.update();

  if (!ArduinoCloud.connected()) {
    Serial.println("Internet is not connected");
    delay(10000);
  } else {
    // Get the current time
    unsigned long currentTime = ArduinoCloud.getLocalTime();
    
    // Get the current date and time
    DateTime dateTime = getCurrentDateTime(currentTime);

    // Print the current date and time
    Serial.print("Current date and time: ");
    Serial.print(dateTime.year);
    Serial.print("-");
    if (dateTime.month + 1 < 10) {
      Serial.print("0");
    }
    Serial.print(dateTime.month + 1);
    Serial.print("-");
    if (dateTime.day < 10) {
      Serial.print("0");
    }
    Serial.print(dateTime.day);
    Serial.print(" ");
    Serial.print(dateTime.hour);
    Serial.print(":");
    if (dateTime.minute < 10) {
      Serial.print("0");
    }
    Serial.println(dateTime.minute);

    // Display the hour and minute on the TM1637
    tm.display(0, dateTime.hour / 10);
    tm.display(1, dateTime.hour % 10);
    tm.display(2, dateTime.minute / 10);
    tm.display(3, dateTime.minute % 10);
    tm.point(true); // Display the colon

    delay(1000);
  }
}

