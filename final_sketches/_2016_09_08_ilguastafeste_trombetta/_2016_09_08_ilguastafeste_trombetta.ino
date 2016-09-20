/*
 *  8 September 2016
 *  Il Guastafeste - Trombetta
 *  Ivan Iovine
 *  
 *  Casa Jasmina / Maker Faire Rome
 *
 */

 
#include <Servo.h>
//Include Libraries for WLAN and MQTT Communication
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <ArduinoCloud.h>

// Library for Thermal Printer
#include "Adafruit_Thermal.h"

// Libraries for Audio File
#include <SPI.h>
#include <SD.h>
#include <AudioZero.h>

//Libraries for Hours - RTC
#include <RTCZero.h>

// build a wifi object
WiFiSSLClient sslClient;

// build a RTCZero object
RTCZero rtc;

// build a UDP Connection object
WiFiUDP Udp;

// build a new object "ilguastafestetrombetta"
ArduinoCloudThing ilguastafestetrombetta;

// Servo object
Servo servoguastafeste;  // create servo object to control a servo


Adafruit_Thermal printer(&Serial1);
#define TX_PIN 1 // Arduino transmit  PIN 14
#define RX_PIN 0 // Arduino receive

// Variables for WLAN Communication
char *ssid = "Sticazzi";
char *pass = "87327394521381619959";

// Arduino Cloud settings and credentials
const char userName[]   = "dummknopf";
const char thingName[] = "ilguastafestetrombetta";
const char thingId[]   = "c8f20740-561f-4991-ba06-c0cc8ed349b1";
const char thingPsw[]  = "687e8869-815f-4d25-965d-b166b6a9b0da";

// UDP setup
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
const int GMT = 2; //change this to adapt it to your time zone   hours*minutes*seconds

// Set Il Guastafeste Alarm
int hours = 13;
int minutes = 0;
int seconds = 0;
boolean after = false; // Check if time is after 23 hours


// Messages to Print
boolean message = false; // Check if it's time to print the message
String stepOne = "It's too Loud, please be Quiet!";
String stepTwo = "This is the last Warning!";
String stepThree = "The Party is over, Good Night!"; 

// twelve servo objects can be created on most boards
int pos = 90;    // variable to store the servo position
boolean stop_servo = true;

// millis variables for average noise value
unsigned long previousMillis = 0; 
unsigned long currentMillis;
const long interval = 30000;

// Variables for the Microphone
float totValue;
float loudValue;
unsigned int sample;
int peakRaw;
int countLoud = 0;



void setup() {
  
  Serial.begin (9600); 

  // attempt to connect to WiFi network:
  //Serial.print("Attempting to connect to WPA SSID: ");
  //Serial.println(ssid);
  
  // Check if the MKR1000 is connected on WIFI
  if(WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // unsuccessful, retry in 4 seconds
    Serial.print("failed ... ");
    delay(4000);
    Serial.print("retrying ... ");
  }
  else
  {
    //
  }
    //Begin Printer Serial
    Serial1.begin(9600);
    
    //Begin Printer Library
    printer.begin(250);

    //Begin Time the libraries and Functions
    rtc.begin();
    GetCurrentTime();

    // Begin MQTT Communication
    ilguastafestetrombetta.begin(thingName, userName, thingId, thingPsw, sslClient);
    ilguastafestetrombetta.enableDebug();
    
    // define the properties
    ilguastafestetrombetta.addProperty("ilguastafestemaster", CHARSTRING, RW);
    ilguastafestetrombetta.addExternalProperty("ilguastafestestereo","ilguastafestestereoslave",CHARSTRING);
    
     if(!SD.begin(4)) {
      Serial.println("failed!");
     }
     else
     {
      Serial.println("Valid SD Card");
     }

       // Init Servo
    //servoguastafeste.attach(5);  // attaches the servo on pin 5 to the servo object


  
}

void loop() {
  
  currentMillis = millis();
  peakRaw = peak();
    
  ilguastafestetrombetta.poll();
  
    switch (countLoud) {
      case 0:
        
      totValue += peakRaw;
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
        Serial.println(totValue);
        Serial.println(interval);
        loudValue = (interval/totValue)*1000;
        Serial.println(loudValue);
    
        if(loudValue <= 500 && loudValue >= 1)
        {
            // Turn Down Stereo Volume
            ilguastafestetrombetta.writeProperty("ilguastafestemaster", "loud");
            
            // Change here the proprietes to manipulate the servo
            stop_servo = false; 
            if(stop_servo == false)
            {
              servoMotor(true,500,20);
              stop_servo = true;
            }
    
            message = true;
            
            if(message == true)
            {
              // Print the message
              printMessage(stepOne);
              Serial.println("Print Message!");
              message = false;
            }

            countLoud = 1;     
        }
        else
        {
          // Change here the proprietes to manipulate the servo
          stop_servo = false; 
          if(stop_servo == false)
          {
            servoMotor(false,500,0);
            stop_servo = true;
          }
          restart();
          Serial.println("COUNTLOUD 0");
       
        }
          totValue = 0; 
      }
      
      break;
      case 1:

      totValue += peakRaw;
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
        Serial.println(totValue);
        Serial.println(interval);
        loudValue = (interval/totValue)*1000;
        Serial.println(loudValue);
        
        if(loudValue <= 500 && loudValue >= 1)
        {
        
            // Turn Down Stereo Volume
             ilguastafestetrombetta.writeProperty("ilguastafestemaster", "tooloud");
             
             // Change here the proprietes to manipulate the servo
            stop_servo = false; 
            if(stop_servo == false)
            {
              servoMotor(true,500,20);
              stop_servo = true;
            }
    
             message = true;
            
            if(message == true)
            {
               // Print the message
               printMessage(stepTwo);
               Serial.println("Print Message!");
               message = false;
            } 

            countLoud = 2;
        }
        else
        {
          
          // Change here the proprietes to manipulate the servo
          stop_servo = false; 
          if(stop_servo == false)
          {
            servoMotor(false,500,20);
            stop_servo = true;
          }
          restart();
          Serial.println("COUNTLOUD 0");
  
        }
          totValue = 0;
      }
           
      break;
      case 2:

      totValue += peakRaw;
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
        Serial.println(totValue);
        Serial.println(interval);
        loudValue = (interval/totValue)*1000;
        Serial.println(loudValue);
        
        if(loudValue <= 500 && loudValue >= 1)
        {

            // Turn Out Lights and Stereo
            ilguastafestetrombetta.writeProperty("ilguastafestemaster", "partyisover");

            stop_servo = false; 
            if(stop_servo == false)
            {
              // Change here the proprietes to manipulate the servo
              servoMotor(true,500,20);
              stop_servo = true;
            }
                      
            message = true;
            
            if(message == true)
            {
              // Print the message
              printMessage(stepThree);
              Serial.println("Print Message!");
              message = false;
            }
            delay(2000);
            //Set Volume Speaker
            digitalWrite(1, HIGH);
            digitalWrite(2, LOW);
            digitalWrite(3, LOW);
            //Play the final sound
            finalAudio();
    
            // Restart the Sketch!
            countLoud = 3;
        }
        else
        {
          
          // Change here the proprietes to manipulate the servo
          stop_servo = false; 
          if(stop_servo == false)
          {
            servoMotor(false,1000,20);
            stop_servo = true;
          }
          restart();
          Serial.println("COUNTLOUD 0");
          
        }
          
          totValue = 0; 
        
        }
        
      break;
      case 3:

      totValue += peakRaw;
      if (currentMillis - previousMillis >= interval) 
      {
              delay(26000);
              // Change here the proprietes to manipulate the servo
              stop_servo = false; 
              if(stop_servo == false)
              {
                servoMotor(false,1500,20);
                stop_servo = true;
              }
              restart();
      }
      
      break;
      default:
        // Nothing happens here
      break;
    }
  }


//Printer Function
void printMessage(String stepMessage)
{
  
  printer.setSize('S');
  printer.justify('C');
  printer.println(stepMessage);
  printer.feed(5);
}

// Audio Speaker Function
void finalAudio()
{
   AudioZero.begin(44100);
   File audioFile = SD.open("tt.wav"); // open wave file from sdcard
    if (!audioFile) {
      Serial.println("error opening tt.wav");
    }
    AudioZero.play(audioFile);
    AudioZero.end();
}

// Take Raw Data every 50 ms from Microphone
int peak()
{
    unsigned long startMillis= millis();  // Start of sample window
    int peakToPeak = 0;   // peak-to-peak level
     
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

     
       // collect data for 50 mS
       while (millis() - startMillis < 50)
       {
          sample = analogRead(A1);
          //Serial.println(sample);
          if (sample < 1024)  // toss out spurious readings
          {
             if (sample > signalMax)
             {
                signalMax = sample;  // save just the max levels
             }
             else if (sample < signalMin)
             {
                signalMin = sample;  // save just the min levels
             }
          }
       }
       peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
       return peakToPeak;
       Serial.println(peakToPeak);
       delay(500);
}

void restart()
{

     //Restart State
     ilguastafestetrombetta.writeProperty("ilguastafestemaster", "restart");
     delay(2000);
     countLoud = 0;
     totValue = 0; 
     Serial.println("Reset!");
     

}

void servoMotor(boolean forward,int m,int vel)
{
  if(forward == true)
  {
    servoguastafeste.attach(5);
    Serial.println("Run Run Run!");
    servoguastafeste.write(pos - vel);
    delay(m);
    servoguastafeste.detach();
    
  }
  else
  {
    servoguastafeste.attach(5);
    Serial.println("Run Run Run!");
    servoguastafeste.write(pos + vel);
    delay(m);
    servoguastafeste.detach();   
  }

}

