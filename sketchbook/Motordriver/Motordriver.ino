/*
 * Motordriver:  
 *  
 * Functional description:
 *   - on start device connects to WiFi and initialises MotorShield
 *   - it listens on WIFI TODO FIXME HU
 *   
 *   
 *   - when button is pressed, LED is flashed for 0.25 seconds and order 
 *     message is published
 *   - when status message from backend is received, LED is flashed 
 *     for 2 seconds to give the user feedback that the order is processed.
 * -------------------------------------------------------------------------
 * This is DEMO source code, demonstrating some basics and principles
 * used in IoT devices (e.g. MQTT, interfacing with hardware input and 
 * outputs). In a real world application we would likely use a REST 
 * interface to the backend service and also need some kind of personal-
 * isation procedure to initialize the device. Also note that for reasons
 * of simplicity that this demo does not implement any security measures 
 * like encryption, authentication or authorization.
 * -------------------------------------------------------------------------
 * 
 * see (https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon)
 * 
 */


#include "DualMC33926MotorShield.h"
#include <ESP8266WiFi.h>



// WiFi connection configuration
//#define WIFI_PASSWORD   "LeInternetUwant11"
//#define WIFI_SSID       "Buergernetz-WLAN"
#define WIFI_PASSWORD   "Welpenspiel"
#define WIFI_SSID       "TP-Huabas"
// maximum number of tries during wifi connection setup
#define WIFI_MAX_TRIES  30
// delay in ms between checks during wifi connection setup
#define WIFI_DELAY_TRY  1000


/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read
const int ledPin = 13;


//
// global variables
// 
int speedM1 = 0;
int speedM2 = 0;
int loopCount = 0;

WiFiServer server(80);
DualMC33926MotorShield md;


void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println();

  // prepare GPIO
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);
  delay(10);

  Serial.println("\nInitializing Dual MC33926 Motor Shield");
  md.init();
  delay(10);

  Serial.println("\nInitializing Wifi");
  connectWiFi();
  delay(10);

  Serial.println("\nStarting Webserver");
  server.begin();
  delay(10);

  Serial.println("Usage:");
  Serial.println("");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/sheep?speed=<speed>&dir=<dir>");
  Serial.println("");
  Serial.println("where");
  Serial.println("  <speed> : -400 to 400");
  Serial.println("  <dir>   : |speed| + |dir| < 400");
  Serial.println("");

}

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    Serial.print(".");
    loopCount = loopCount+1;
    if (loopCount > 50)
    {
      loopCount =0;
      Serial.println();       
    }
    delay(1000);    
    return;
  }
  loopCount =0;

  // Wait until the client sends some data
  Serial.println("\n\nnew client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int cmdSpeed = 0;
  int cmdDir = 0;

  if (req.indexOf("/sheep") != -1){
    /*
    int locationQuestionmark = req.find("?");
    String params = req.substr(locationQuestionmark+1, req.size());
    int locationEquals = req.find("=");
    String speedString = req.substr(locationEquals+1, req.size());
    int cmdSpeed = std::stoi(speedString);
    */

    int posSpeed = req.indexOf("speed=");
    int posDir = req.indexOf("dir=");
    if (posSpeed != -1)
    {
      String speedString = req.substring(posSpeed+6, req.length());
      cmdSpeed =  speedString.toInt();
    }
    if (posDir != -1)
    {
      String dirString = req.substring(posDir+4, req.length());
      cmdDir =  dirString.toInt();
    }

  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  adjustSpeedM1(cmdSpeed + cmdDir);
  adjustSpeedM2(cmdSpeed - cmdDir);

  Serial.print("M1: ");
  Serial.print(md.getM1CurrentMilliamps());
  Serial.print(" M2: ");
  Serial.println(md.getM2CurrentMilliamps());

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nSpeed-dir is now ";
  s += md.getM1CurrentMilliamps();
  s += " - ";
  s += md.getM2CurrentMilliamps();
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
  
  // delay(1000);
}

void adjustSpeedM1(int newSpeed){
  Serial.print("adjustSpeedM1 to  ");
  Serial.println(newSpeed);
  if (newSpeed < speedM1){
     decelerate("M1", speedM1, newSpeed*4); 
  }else{
     accelerate("M1", speedM1, newSpeed*4); 
  }
  speedM1 = newSpeed; 
}

void adjustSpeedM2(int newSpeed){
  Serial.print("adjustSpeedM2 to ");
  Serial.println(newSpeed);
  if (newSpeed < speedM2){
     decelerate("M2", speedM2, newSpeed*4);
  }else{
     accelerate("M2", speedM2, newSpeed*4); 
  }
  speedM2 = newSpeed; 
}

void accelerate(String motor, int from, int to){  
  for (int i = from; i < to; i++)
  {
    if (motor.equals("M1")){
      md.setM1Speed(i);
    }else{
      md.setM2Speed(i);
    }
    delay(2);
  }
}

void decelerate(String motor, int from, int to){  
  for (int i = from; i > to; i--)
  {
    if (motor.equals("M1")){
      md.setM1Speed(i);
    }else{
      md.setM2Speed(i);
    }
    delay(2);
  }
}


void connectWiFi()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());
}


