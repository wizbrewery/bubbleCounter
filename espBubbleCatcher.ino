//#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>


/*
 ESP8266 Blink by Simon Peter
 Blink the blue LED on the ESP-01 module
 This example code is in the public domain
 
 The blue LED on the ESP-01 module is connected to GPIO1 
 (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 
 Note that this sketch uses BUILTIN_LED to find the pin with the internal LED
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);
WiFiClient client;

#define resolution 800000 //only capture pulse that are greater than this to avoid noise
#define sample 2 // sample size of pulses for average starts at 0

volatile unsigned long pulseThen;
volatile unsigned long pulseNow;
volatile unsigned long numPulses;
volatile unsigned long keeppulse[sample];
volatile int count=0;
volatile bool changed = true;
String tempInt=" ";

unsigned long myChannelNumber = "Enter you channe number here no quotes";
String writeAPIKey = "Insert your writeAPIKey here";
char thingSpeakAddress[] = "api.thingspeak.com";
const int updateThingSpeakInterval = 16 * 1000; // Time interval in millisecond

const char WiFiSSID[] = "Insert you wireless SSID here.  Keep quotes";
const char WiFiPSK[] = "Insert you wireless password here.  Keep quotes";

int LED_PIN = 4;
unsigned long lastConnectionTime = millis();
int failedCounter = 0;

void isr()
{
 unsigned long now = micros();
 float pulselength;
 pulseNow = now;
 int averagepulse;
 int totalsample = 0;

 display.clearDisplay();
 if ((pulseNow - pulseThen) > resolution)
 {
   Serial.print("numPulses ");
   Serial.print(numPulses+1);
   Serial.print(" ");
   pulselength = (float)(pulseNow - pulseThen) /1000000;
   Serial.print(pulselength);
//   Serial.print(" ");
//   Serial.print("keeppulse ");
//   Serial.print(count);
   Serial.print("\n");
   tempInt = String(pulselength);
   if( count <= sample )
   {
    keeppulse[count] = pulselength;
    count++;
   }
   else
   {
    count = 0;
    Serial.print("Average in a sample of ");
    Serial.print(sample);
    Serial.print(" is ");
    for( int x=1; (x<=sample); x++ ) {
      totalsample += (int)keeppulse[x];
      Serial.print(keeppulse[x]);
      Serial.print(" ");
     }

    averagepulse = totalsample/sample;
    
    Serial.print("Total ");
    Serial.print(totalsample);
    Serial.print(" ");
    Serial.print("Average ");
    Serial.print(averagepulse);
    Serial.print("\n\n");
    display.print(averagepulse);
    display.print("secs ");
    }
      
   pulseThen = pulseNow;
   ++numPulses;
   changed=false;
 }
}

void updateThingSpeak(String tsData) {
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    lastConnectionTime = millis();

    if (client.connected()) {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
    }
  }
}

void connectWiFi()
{
  byte ledStatus = LOW;

  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);
  Serial.print("wifi connecting ");

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    Serial.print(".");
    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
}

void setup() {
//pinMode(LED_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
   Serial.begin(115200);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
   digitalWrite(LED_PIN, LOW); //turn off on board led
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // init done
//  WiFi.begin(ssid, password);
  Serial.println("");
  connectWiFi();

  // Wait for connection
/*  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
*/  
  display.display();
  delay(2000);
  display.clearDisplay(); 
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

    // text display tests
   display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(2,10);
  display.println("Jack :) ..");
    display.display();
  delay(2000);
  display.clearDisplay();
 Serial.println("Wizbrewery Bubble Counter\n");
 pinMode(2, OUTPUT); //probe output on pin D4
 attachInterrupt(0, isr, RISING);    // define interrupt 0
 numPulses = 0;                      // prime the system to start a new reading
// ThingSpeak.begin(client);
}

// the loop function runs over and over again forever
void loop() {
  //detect changed reading and drop any that are shorter than updateThingSpeakInterval
  if(!changed && (millis() - lastConnectionTime > updateThingSpeakInterval)){
    updateThingSpeak("field1="+tempInt);
    changed=true;
  }
}
