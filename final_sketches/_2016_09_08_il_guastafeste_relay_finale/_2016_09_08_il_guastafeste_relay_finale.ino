/*
 * 8 September 2016
 * Il Guastafeste - Light Control via Relay
 * Ivan Iovine
 * 
 * Call for residency - Casa Jasmina / Maker Faire Rome
 */

 
#include <WiFi101.h>
#include <ArduinoCloud.h>

// Variables for WLAN Communication
char *ssid = "";
char *pass = "";

// Arduino Cloud settings and credentials
const char userName[]   = "dummknopf";
const char thingName[] = "ilguastafesteluce";
const char thingId[]   = "7ef31755-9535-44bd-907a-aa585c087498";
const char thingPsw[]  = "79afed4a-0ea1-4e64-b0d1-f6b18777e87d";


// build a object wifi client
WiFiSSLClient sslClient;


// build a new object "ilguastafesteluce"
ArduinoCloudThing ilguastafesteluce;

int pinRelay = 6;    // variable to store the relay position

void setup() {
  Serial.begin(9600);

   // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // unsuccessful, retry in 4 seconds
    Serial.print("failed ... ");
    delay(4000);
    Serial.print("retrying ... ");
  }
  
  pinMode(pin, OUTPUT);
  digitalWrite(pin,HIGH);

  ilguastafesteluce.begin(thingName, userName, thingId, thingPsw, sslClient);
  ilguastafesteluce.enableDebug();
   // define the properties
  ilguastafesteluce.addProperty("ilguastafesteluceslave", CHARSTRING, RW);
  ilguastafesteluce.addExternalProperty("ilguastafestetrombetta","ilguastafestemaster",CHARSTRING);
}

void loop() {
  
ilguastafesteluce.poll();
Serial.println(ilguastafesteluce.readProperty("ilguastafestetrombetta","ilguastafestemaster"));

if (ilguastafesteluce.readProperty("ilguastafestetrombetta","ilguastafestemaster") == "partyisover") 
{

  digitalWrite(pinRelay,LOW);

}
else
{
  digitalWrite(pinRelay,HIGH);
}

}

