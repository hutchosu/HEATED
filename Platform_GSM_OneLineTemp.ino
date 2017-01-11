/*
 * Any comments which begin with *** are debug commands.
 * 
 * Custom functions:
 *  void convertGPS() --- convert date, time, lat, long to desired format
 *  void dweetData() --- send data via dweet.io protocol (compiles data into URL address, which is then called by GSM board; data can be retrieved using a "listening" URL address)
 */
#include "Adafruit_FONA.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// standard pins for the shield, adjust as necessary
#define FONA_RX 2
#define FONA_TX 10
#define FONA_RST 4
#define ONE_WIRE_BUS 34

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Have a FONA 3G? use this object type instead
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress thermo1 = { 0x28, 0xFF, 0x59, 0x69, 0x56, 0x16, 0x04, 0xA0 };
DeviceAddress thermo2 = { 0x28, 0xFF, 0xA0, 0x41, 0x53, 0x16, 0x03, 0x2F };
DeviceAddress thermo3 = { 0x28, 0xFF, 0x0B, 0x41, 0x53, 0x16, 0x03, 0x6C };

float adcValue = 0;
float voltage = 0;
float percentRH = 0;


// Data variables
float GSM_date = 0;
float GSM_time = 0;
float latitude = 0;
float longitude = 0;
float altitude = 0;
float temp_c = 0;
float temp_f = 0;
float rel_humidity = 0;

// dweet.io name - "UHI" followed by last 5 digits of GSM breakout board serial number
String myThingName = "UHI60634";   

void setup() {

  while (! Serial);

  Serial.begin(115200);
  Serial.println(F("Adafruit FONA 808 & 3G GPS demo"));
  Serial.println(F("Initializing FONA... (May take a few seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
  //***  Serial.println(F("Couldn't find FONA"));
    
  }
  //***Serial.println(F("FONA is OK"));
  // Try to enable GPRS
  

  //***Serial.println(F("Enabling GPS..."));
  //while (!fona.enableGPS(true)) {

    //GPS not enabled
    
  //}

  fona.setGPRSNetworkSettings(F("freedompop.foggmobile.com"), F(""), F(""));
  Serial.println(fona.enableGPRS(false));
  delay(3000);
  while (!fona.enableGPRS(true)) {

    //GPRS not enabled
    
  }

  // Start up the temp library
  sensors.begin();
  // set the resolution to 12 bit (good enough?)
  sensors.setResolution(thermo1, 12);
  sensors.setResolution(thermo2, 12);
  sensors.setResolution(thermo3, 12);

}

void loop() {
  delay(5000);

  float speed_kph, heading;

  // Retrieve position values from GSM board - speed_kph, heading are not currently used
  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

  tempRead();
  humRead();

  // TODO: retrieve date and time from GSM board

  if (gps_success) {

    dweetData();
    
//    Serial.print("GPS lat:");
//    Serial.println(latitude, 6);
//    Serial.print("GPS long:");
//    Serial.println(longitude, 6);
//    Serial.print("GPS speed KPH:");
//    Serial.println(speed_kph);
//    Serial.print("GPS heading:");
//    Serial.println(heading);
//    Serial.print("GPS altitude:");
//    Serial.println(altitude);
//    Serial.print("C temp: ");
//    Serial.println(temp_c);
//    Serial.print("F temp" ");
//    Serial.println(temp_f);
//    Serial.print("Humidity: ");
//    Serial.println(rel_humidity);

  } else {
//    //***Serial.println("Waiting for FONA GPS 3D fix...");
  }
}

void tempRead () {

  sensors.requestTemperatures();
  float temp1=sensors.getTempC(thermo1);
  float temp2=sensors.getTempC(thermo2);
  float temp3=sensors.getTempC(thermo3);
  float avg=(temp1+temp2+temp3)/3.0;
  float tempf=(avg*1.8)+32;

  temp_c = avg;
  temp_f = tempf;
}

void humRead () {
  
  adcValue = analogRead(0); // Read voltage coming from sensor (adcValue will be between 0-1023)
  voltage = (adcValue/1023.0)*5.0; // Translate ADC value into a voltage value
  percentRH = (voltage-0.958)/0.0307; // Translate voltage into percent relative humidity

  rel_humidity = percentRH;
}


void convertGPS() {
  
}

void dweetData() {

  String dweetURL;
  dweetURL = "http://dweet.io/dweet/for/" + myThingName;
  dweetURL += "?date=" + String(GSM_date);
  dweetURL += "&time=" + String(GSM_time);
  dweetURL += "&latidtude=" + String(latitude);
  dweetURL += "&longitude=" + String(longitude);
  dweetURL += "&altitude=" + String(altitude);
  dweetURL += "&tempC=" + String(temp_c);
  dweetURL += "&tempF=" + String(temp_f);
  dweetURL += "&relHumid=" + String(rel_humidity);
  
  char buf[1000];
  dweetURL.toCharArray(buf, dweetURL.length());

  uint16_t statusCode;
  int16_t length;
  
  if (!fona.HTTP_GET_start(buf, &statusCode, (uint16_t *)&length)) {
    
  }
  fona.HTTP_GET_end();
}

