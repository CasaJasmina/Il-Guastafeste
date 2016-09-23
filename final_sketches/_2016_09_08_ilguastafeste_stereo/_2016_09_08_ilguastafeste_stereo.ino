/*
 *  8 September 2016
 *  Il Guastafeste - Stereo
 *  Ivan Iovine
 *  
 *  Call for Residency - Casa Jasmina / Maker Faire Rome 
 */
#include <WiFi101.h>
#include <ArduinoCloud.h>

// variables for WLAN Communication
char *ssid = "";
char *pass = "";

// arduino cloud settings and credentials
const char userName[]   = "dummknopf";
const char thingName[] = "ilguastafestestereo";
const char thingId[]   = "4f9ccd39-013d-4619-a526-6fa5545b50d7";
const char thingPsw[]  = "9b4510e3-31f3-4083-9aa7-d69bfc8b72f3";

// variable to set the gain
int pinGain0 = 5;    
int pinGain1 = 4; 

int shutRight = 3;    // variable to shut down the right speaker
int shutLeft = 2;    // variable to shut down the left speaker

int potVal;         // variable to read the potentiometer

//Check Variables for time and set state
int step1 = 0;
int step2 = 0;
int step3 = 0;
int countState = 0;

// build a new object for the wifi client
WiFiSSLClient sslClient;


// build a new object "ilguastafestestereo"
ArduinoCloudThing ilguastafestestereo;

void setup() {
  
  Serial.begin(9600);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // unsuccessful, retry in 4 seconds
    Serial.print("failed ... ");
    delay(4000);
    Serial.print("retrying ... ");
  }

  ilguastafestestereo.begin(thingName, userName, thingId, thingPsw, sslClient);
  ilguastafestestereo.enableDebug();
   // define the properties
  ilguastafestestereo.addProperty("ilguastafestestereoslave", CHARSTRING, RW);
  ilguastafestestereo.addExternalProperty("ilguastafestetrombetta","ilguastafestemaster",CHARSTRING);
}

void loop() {

  potVal = map(analogRead(A1),0,1023,1023,0);
  ilguastafestestereo.poll();
  countState = setStates();
  
  switch(countState){
    case 0:

     step1 = 0;
     step2 = 0;
     digitalWrite(shutRight, HIGH);
     digitalWrite(shutLeft, HIGH);
     digitalWrite(pinGain0, LOW);
     digitalWrite(pinGain1, LOW);
     setVolume(potVal);
     Serial.println("Let's Start!");
     
    break;
    case 1:
    
     step1++;
     Serial.println(step1);
     if(step1 < 15000)
     {  
        digitalWrite(shutRight, HIGH);
        digitalWrite(shutLeft, HIGH);
        analogWrite(pinGain0, HIGH);
        analogWrite(pinGain1, HIGH);
        Serial.println("Step 1");
     }
     else
     {
        setVolume(potVal);
     }
   break;
   case 2:
   
     step2++;
     Serial.println(step2);
     if(step2 < 15000)
     {
        digitalWrite(shutRight, HIGH);
        digitalWrite(shutLeft, HIGH);
        analogWrite(pinGain0, HIGH);
        analogWrite(pinGain1, HIGH);
        Serial.println("Step 2");
     }
     else
     {
        setVolume(potVal);
     }
     
   break;
   case 3:

        analogWrite(shutRight, HIGH);
        analogWrite(shutLeft, HIGH);
        Serial.println("Step 3");
        delay(2000);
        step1 = 0;
        step2 = 0;
        
   break;
   default:
   // Nothing to do here
   break;
  }
  
  Serial.println(countState);
 
}


int setStates()
{

   //Serial.println(ilguastafestestereo.readProperty("ilguastafestetrombetta","ilguastafestemaster"));
   if(ilguastafestestereo.readProperty("ilguastafestetrombetta","ilguastafestemaster") == "loud")
   {
       return 1;
   }
   else if(ilguastafestestereo.readProperty("ilguastafestetrombetta","ilguastafestemaster") == "tooloud")
   {
       return 2;
   } 
   else if(ilguastafestestereo.readProperty("ilguastafestetrombetta","ilguastafestemaster") == "partyisover")
   {
        return 3;
   }
   else if(ilguastafestestereo.readProperty("ilguastafestetrombetta","ilguastafestemaster") == "restart")
   {
        return 0;
   }
   else
   {
        return 0;
   }

}


void setVolume(int p)
{
   if(p >= 0 && p < 15)
   {
     Serial.println("Shut Down");
     //Shut Down  
     analogWrite(shutRight, HIGH);
     analogWrite(shutLeft, HIGH);
   }
   else if(p > 15 && p < 265)
   {
      Serial.println("LOW Volume");
      //LOW Volume
      analogWrite(pinGain0, HIGH);
      analogWrite(pinGain1, HIGH);
      digitalWrite(shutRight, HIGH);
      digitalWrite(shutLeft, HIGH);
      
   }
   else if(p > 265 && p < 520)
   {
       Serial.println("MID-LOW Volume");
       //MID-LOW Volume
       analogWrite(pinGain0, HIGH);
       digitalWrite(pinGain1, LOW);
       digitalWrite(shutRight, HIGH);
       digitalWrite(shutLeft, HIGH);

    
   }
   else if(p > 520 && p < 770)
   {
       Serial.println("MID-HIGH Volume");
       //MID-HIGH Volume
       digitalWrite(pinGain0, LOW);
       analogWrite(pinGain1, HIGH);
       digitalWrite(shutRight, HIGH);
       digitalWrite(shutLeft, HIGH);
    
   }
   else if (p > 770 && p < 1024)
   {
       Serial.println("HIGH Volume");
       //HIGH Volume
       digitalWrite(pinGain0, LOW);
       digitalWrite(pinGain1, LOW);
       digitalWrite(shutRight, HIGH);
       digitalWrite(shutLeft, HIGH);
   }
   else
   {
    // Nothing to do here
   }
  
}

