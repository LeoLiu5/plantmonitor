// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#if defined(ARDUINO_ARCH_ESP32)
  // ESP32Servo Library (https://github.com/madhephaestus/ESP32Servo)
  // installation: library manager -> search -> "ESP32Servo"
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

#include <ESP8266WiFi.h>        // Include the Wi-Fi library

const char* ssid     = "CE-Hub-Student";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "casa-ce-gagarin-public-service";     // The password of the Wi-Fi network
  
/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// pin used to control the servo
#define SERVO_PIN 2

// create an instance of the servo class
Servo servo;


// pin connected to DH22 data line
#define DATA_PIN 12
uint8_t soilPin = 0; // ADC or A0 pin on Huzzah
int Moisture = 1; // initial value just in case web page is loaded before readMoisture called
int sensorVCC = 13;
char msg[50];
unsigned long previousMillis = 0UL;
unsigned long interval = 60000UL;
// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature' and 'humidity' feeds
// set up the 'servo' feed
AdafruitIO_Feed *servo_feed = io.feed("servo");
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *moisture = io.feed("moisture");
void setup() {
Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');


  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);
    // tell the servo class which pin we are using
  servo.attach(SERVO_PIN);

  // initialize dht22
  pinMode(DATA_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Set up the outputs to control the soil sensor
  // switch and the blue LED for status indicator
  pinMode(sensorVCC, OUTPUT); 
  digitalWrite(sensorVCC, LOW);

  // set up a message handler for the 'servo' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  servo_feed->onMessage(handleMessage);
  
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  servo_feed->get();
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval)
  {
	/* The Arduino executes this code once every second
 	*  (interval = 1000 (ms) = 1 second).
 	*/
  TempHumMoi();
 	// Don't forget to update the previousMillis value
 	previousMillis = currentMillis;
}}

void readMoisture(){

  // power the sensor
  digitalWrite(sensorVCC, HIGH);

  delay(100);
  // read the value from the sensor:
  Moisture = analogRead(soilPin);         
  //Moisture = map(analogRead(soilPin), 0,320, 0, 100);    // note: if mapping work out max value by dipping in water     
  //stop power
  digitalWrite(sensorVCC, LOW);  
  // delay(100);
  Serial.print("moisture: ");
  Serial.println(Moisture);   // read the value from the nails
  snprintf (msg, 50, "%.0i", Moisture);
  Serial.println(msg);
}


void handleMessage(AdafruitIO_Data *data) {

  // convert the data to integer
  int angle = data->toInt();

  // make sure we don't exceed the limit
  // of the servo. the range is from 0
  // to 180.
  if(angle < 0)
    angle = 0;
  else if(angle > 180)
    angle = 180;

  servo.write(180 - angle);
}


void TempHumMoi() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(celsius);

  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  readMoisture();
  // save moisture to Adafruit IO
  moisture->save(Moisture);
  // wait 5 seconds (5000 milliseconds == 5 seconds)
  // delay(60000);
  if (event.relative_humidity<50 || Moisture<20 || celsius<18 || celsius>28){  
  servo.write(0);  // tell servo to go to a particular angle
  delay(1000);
  
  servo.write(90);              
  delay(500); 
  
  servo.write(135);              
  delay(500);
  
  servo.write(180);              
  delay(1500);
  
  servo.write(0);  // tell servo to go to a particular angle
  delay(1000);
  
  servo.write(90);              
  delay(500); 
  
  servo.write(135);              
  delay(500);
  
  servo.write(180);              
  delay(1500);
  
  servo.write(0);  // tell servo to go to a particular angle
  delay(1000);
  
  servo.write(90);              
  delay(500); 
  
  servo.write(135);              
  delay(500);
  
  servo.write(180);              
  delay(1500);

  servo.write(0);  // tell servo to go to a particular angle
  delay(1000);
  
  servo.write(90);              
  delay(500); 
  
  servo.write(135);              
  delay(500);
  
  servo.write(180);              
  delay(1500);     

  servo.write(0);  // tell servo to go to a particular angle
  delay(1000);
  
  servo.write(90);              
  delay(500); 
  
  servo.write(135);              
  delay(500);
  
  servo.write(180);              
  delay(1500);}            
  // digitalWrite(SERVO_PIN,HIGH);
  // delayMicroseconds(1450);
  // digitalWrite(SERVO_PIN,LOW);
  // delayMicroseconds(18550);
}