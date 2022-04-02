#include <Arduino.h>
#include <timeLib.h> 
#include <WiFi.h>

long timezone = 8; 
byte daysavetime = 0;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

 
bool syncNTP(){
    Serial.println("Contacting Time Server");
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	struct tm tmstruct ;
    delay(1000);
    yield();
    delay(1000);
    yield();
    tmstruct.tm_year = 0;
    if ( getLocalTime(&tmstruct, 5000) ) {
	    //Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        //Serial.println("");
        printLocalTime();
        setTime(mktime(&tmstruct));
        adjustTime(3600*timezone);
        log_w("Time:%02d:%02d:%02d", hour(), minute(), second());
        return true;
        }
    else    
        return false;    
}