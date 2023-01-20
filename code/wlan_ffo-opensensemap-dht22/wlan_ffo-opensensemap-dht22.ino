/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */
 //dieser Sketch wurde in Zusammenarbeit von @dertica und @klimapower-ho4 erstellt.
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <Wire.h>

/* hier wurde z.B.der PIN 14 gewählt. falls Ihr einen anderen PIN verwendet, 
diese Zahl in der nächsten Zeile ändern. Datenblatt hier erhältlich: https://makesmart.net/esp8266-d1-mini-datenblatt/ */
#define DHTPIN 14     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Der Wert der Luftfeuchtigkeit kann hier korrigiert werden
float HumidityCalibrate = -10;

DHT dht(DHTPIN, DHTTYPE);

 IPAddress myOwnIP; // ownIP for mDNS 

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

  void setup(){ // Einmalige Initialisierung
 
  Serial.begin(115200);
  dht.begin();
 
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
  myOwnIP = WiFi.localIP();

  delay( 30000 );

  }

  void loop() { // Kontinuierliche Wiederholung 
//Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
//Read temperature as Celsius
  float t = dht.readTemperature();

//bei float h2 = h + HumidityCalibrate, hier die Differenz der Kalibrierung/Justierung eintragen
  float h2 = h +0.0;

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
     Serial.print("Humidity: "); 
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C ");
  
    sendSenseMap("ingress.opensensemap.org",h2,1,"12Dummy34","12Dummy56");// Post to OpenSenseMap
    sendSenseMap("ingress.opensensemap.org",t,1,"12Dummy34","12Dummy78");// Post to OpenSenseMap
   }
  
  delay(1800000);
// ESP.deepSleep(30e6, WAKE_RF_DEFAULT); 
//1800000
//ESP.deepSleep( (long)10000*1000UL,WAKE_RF_DEFAULT);//Tiefschlaf, danach Reset und von vorn
} //end loop
