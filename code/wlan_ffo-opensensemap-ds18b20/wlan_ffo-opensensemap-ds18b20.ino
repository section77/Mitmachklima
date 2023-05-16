/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */

 //dieser Sketch wurde in Zusammenarbeit von @dertica und @klimapower-ho4 erstellt.
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Der PIN D4 (GPIO 2) wird als BUS-Pin verwendet
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

// In dieser Variable wird die Temperatur gespeichert
float temperature;

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

  // DS18B20 initialisieren
  DS18B20.begin();

  String WlanName = "ortenau.freifunk.net";
  String WlanPasswort = "";
 
  //------------ WLAN initialisieren 
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print ("\nWLAN connect to:");
  Serial.print(WlanName);
  WiFi.begin(WlanName,WlanPasswort);
  while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
    delay(500); 
    Serial.print(".");
  };
  Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
  myOwnIP = WiFi.localIP();

  delay( 1000 );

  }

void loop() { // Kontinuierliche Wiederholung 

  DS18B20.requestTemperatures();
  temperature = DS18B20.getTempCByIndex(0)-0.0;

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature)) {
    Serial.println("Failed to read from DS18B20 sensor!");
  } else {
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println(" *C ");
  
    sendSenseMap("ingress.opensensemap.org",temperature,1,"opensensemap-Box-ID","opensensemap-Sensor-ID");// Post to OpenSenseMap
   }
  
  delay(600000);
// ESP.deepSleep(30e6, WAKE_RF_DEFAULT); 
//1800000
//ESP.deepSleep( (long)10000*1000UL,WAKE_RF_DEFAULT);//Tiefschlaf, danach Reset und von vorn
} //end loop
