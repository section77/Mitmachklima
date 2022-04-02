/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */
#include <ESP8266WiFi.h>
#include <Seeed_HM330X.h>
#include <ESP8266HTTPClient.h>
#include <SparkFunBME280.h>
#include <Wire.h>

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

IPAddress myOwnIP; // ownIP for mDNS 

int PM25 = 0 ;
HM330X HM330sensor; // www.seeed.cc, Author: downey

int HM330sensor_ready=0; // for init after deep-sleep

// Feinstaubsensor HM330 Copyright (c) 2018 Seeed Technology Co., Ltd. 
int readFeinstaubHM330(int chan) {
  if(!HM330sensor_ready) {
    HM330sensor_ready = !HM330sensor.init(); 
    delay(1000); 
  }
  u8 data[30]; // Puffer für Antwort
  int i,val;
  u8 sum=0;
  HM330sensor.read_sensor_value(data,29);
  if (chan == 2) i=6; 
  else i=7;
  val = (u16)data[i*2]<<8|data[i*2+1];
  for(int i=0;i<28;i++) { // checksum
    sum+=data[i];
  }
  if(sum!=data[28]) val=-1; 
  return val;
}

int PM10 = 0 ;
//-------------------------OpenSenseMAP  ------ HTTP-POST
void sendSenseMap(String server, float measurement, int digits, String SENSEBOX_ID, String sensorId) {
  HTTPClient http; //Declare object of class HTTPClient
  //Json erstellen
  String jsonValue = "{\"value\":" + String(measurement)+ "}";
  //Mit OSeM Server verbinden und POST Operation durchführen
  String req="http://"+server+"/boxes/"+SENSEBOX_ID+"/"+sensorId;
  Serial.println(req);
  http.begin(req); //Specify request destination
  http.addHeader("Content-Type", "application/json"); //
  int httpCode = http.POST(jsonValue); //Send the request
  String payload = http.getString(); //Get the response payload
  //Serial.print(httpCode);  //Print HTTP return code
  Serial.println(payload); //Print request response payload
  http.end(); //Close connection
}
float Temperatur = 0.0 ;
// Marshall Tylor@sparkfun  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
BME280 boschBME280; // Objekt Bosch Umweltsensor
int boschBME280_ready = 0; // Objekt Bosch Umweltsensor
float relLuftfeuchte = 0.0 ;
float Luftdruck = 0.0 ;
extern "C" {  // zur Nutzung der speziellen ESP-Befehle wie Deep Sleep
#include "user_interface.h"
}


void setup(){ // Einmalige Initialisierung
  boschBME280.setI2CAddress(0x76);  // change I2C address; muss in Arduino (!) im Block "setup" ganz nach oben!
  pinMode( 0 , OUTPUT);
  Serial.begin(115200);
  Wire.begin(); // ---- Initialisiere den I2C-Bus 

  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

  HM330sensor_ready = !HM330sensor.init(); // HM330 Feinstaubsensor 

  boschBME280.settings.runMode = 3; // Normal Mode
  boschBME280.settings.tempOverSample  = 4; 
  boschBME280.settings.pressOverSample = 4;
  boschBME280.settings.humidOverSample = 4;
  boschBME280_ready = boschBME280.begin();
  if (boschBME280_ready == 0) {
    while(1) {
      Serial.println("BME280 nicht vorhanden - der neue Octopus nutzt BME680, ggf. Puzzleteile tauschen"); 
      delay(500);
    }
  }

  digitalWrite( 0 , LOW );

  //------------ WLAN initialisieren 
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print ("\nWLAN connect to:");
  Serial.print("ortenau.freifunk.net");
  WiFi.begin("ortenau.freifunk.net","");
  while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
    delay(500); 
    Serial.print(".");
  };
  Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
  matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
  myOwnIP = WiFi.localIP();
  matrixausgabe_index=0;

  delay( 30000 );

}

void loop() { // Kontinuierliche Wiederholung 
  PM25 = readFeinstaubHM330(2) ;
  PM10 = readFeinstaubHM330(1) ;
  digitalWrite( 0 , HIGH );
  sendSenseMap("ingress.opensensemap.org",PM10,1,"Dummy","Dummy");// Post to OpenSenseMap
  sendSenseMap("ingress.opensensemap.org",PM25,1,"Dummy","Dummy");// Post to OpenSenseMap
  Temperatur = boschBME280.readTempC() ;
  relLuftfeuchte = boschBME280.readFloatHumidity() ;
  Luftdruck = boschBME280.readFloatPressure()/100. ;
  sendSenseMap("ingress.opensensemap.org",Temperatur,1,"Dummy","Dummy");// Post to OpenSenseMap
  sendSenseMap("ingress.opensensemap.org",relLuftfeuchte,1,"Dummy","Dummy");// Post to OpenSenseMap
  sendSenseMap("ingress.opensensemap.org",Luftdruck,1,"Dummy","Dummy");// Post to OpenSenseMap
  ESP.deepSleep( (long)1800000*1000UL,WAKE_RF_DEFAULT);//Tiefschlaf, danach Reset und von vorn
} //end loop
