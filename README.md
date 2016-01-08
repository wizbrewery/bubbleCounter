bubbleCatcher

Arduino IDE

Currently use the version of the Arduino IDE which is supported by the ESP8266 platform, ie. 1.6.5

Setup

Create a folder called bubbleCounter.ino in your arduin sketches folder.
Download bubbleCounter.ino and place it in the above folder

Install the following librarys with your Arduino Library Manager in Sketch > Include Library > Manage Libraries...
ESP8266WiFi.h
WiFiClient.h
WiFiClientSecure.h

Go to http://thingspeak.com, create an account and get an API Key
Edit your bubbleCounter.ino sketch to
Enter the thingSpeak Write API Key
Enter your Wifi SSID and password (ensure to keep quotes)

Compile and upload
