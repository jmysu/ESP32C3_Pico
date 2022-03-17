#include <Arduino.h>
#include <time.h> 
#include <WiFi.h>

long timezone = 8; 
byte daysavetime = 0;


bool checkNTP(){
    //Serial.begin(115200);

    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
        digitalWrite(5, !digitalRead(5));
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Contacting Time Server");
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	struct tm tmstruct ;
    delay(2000);
    tmstruct.tm_year = 0;
    if ( getLocalTime(&tmstruct, 5000) ) {
	    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        Serial.println("");
        return true;
        }
    else    
        return false;    
}

bool setNTP(){
    Serial.println("Contacting Time Server");
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	struct tm tmstruct ;
    delay(1000);
    yield();
    delay(1000);
    yield();
    tmstruct.tm_year = 0;
    if ( getLocalTime(&tmstruct, 5000) ) {
	    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        Serial.println("");
        return true;
        }
    else    
        return false;    
}