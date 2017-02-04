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
#include "HTML.h"
#include <ESP8266WiFi.h>

//
// programm behavior
//

// When there was no command for MAX_ALIVE_DELAY ms, motors stop 
#define MAX_ALIVE_DELAY   5000    

// The loops delay. Each loop delays for at least LOOP_DELAY ms.
#define LOOP_DELAY         20 


// WiFi connection configuration
#define WIFI_PASS1       "Welpenspiel"
#define WIFI_SSID1       "Huabas"
#define WIFI_PASS2       "LeInternetUwant11"
#define WIFI_SSID2       "Buergernetz-WLAN"
#define WIFI_PASS3       "Welpenspiel"
#define WIFI_SSID3       "TP-Huabas"
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
int loopCount = 0;
unsigned long lastMillis = millis();
unsigned long lastCommandTimestamp = millis();
int speedM1 = 0;    // actual speed of motor 1 (PWM)
int speedM2 = 0;    // actual speed of motor 2 (PWM)
int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)
int cmdSpeed = 0;   // desired speed 
int cmdDir = 0;     // desired dir

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

  //
  // print usage on serial monitor
  //
  Serial.println("Usage:");
  Serial.println("");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/sheep?speed=<speed>&dir=<dir>");
  Serial.println("");
  Serial.println("where");
  Serial.println("  <speed> : -100 to 100");
  Serial.println("  <dir>   : |speed| + |dir| < 100");
  Serial.println("");
  Serial.println("or ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/motor?m1=<speedM1>&m2=<speedM2>");
  Serial.println("");
  Serial.println("where");
  Serial.println("  <speedM1> : -100 to 100");
  Serial.println("  <speedM2> : -100 to 100");
  Serial.println("");

  lastMillis = millis();
}

void loop()
{
  unsigned long currentMillis = millis();

  // make every loop take LOOP_DELAY ms
  delay(LOOP_DELAY - (currentMillis - lastMillis));

  //
  // Alive check
  //
  if (currentMillis - lastCommandTimestamp > MAX_ALIVE_DELAY)
  {
    // lost communication -> stop motors
    desiredSpeedM1 = 0;
    desiredSpeedM2 = 0;
  }
    
  //
  // adjust motors
  //
  adjustMotorSpeeds();
 
  //
  // Check if a client has connected
  //
  WiFiClient client = server.available();
  if (!client) {
    lastMillis = millis();
    return;
  }

  //
  // Wait until the client sends some data
  //
  Serial.println("\n\nnew client");
  while(!client.available()){
    delay(1);
  }

  //
  // Read the first line of the request
  //
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //
  // received valid request?
  //
  if (request.indexOf("/sheep") > 0){
    extractSpeedAndDir(request);
  
  }else if (request.indexOf("/motor") > 0){
    extractMotorSpeeds(request);    
    
  }else{
    client.print(page1);
    client.stop();
    lastMillis = millis();
    return;
  }

  // A command was issued, reset alive check timer
  lastCommandTimestamp = millis();
  
  //
  // Send the response to the client
  //
  String s = String("M1: ") + desiredSpeedM1 + " M2: " + desiredSpeedM2; 
  client.flush();
  client.print(s);
  delay(1);
  Serial.println(s);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
  
  lastMillis = millis();
}

/*
 * Match the request, i.e. extract speed and dir
 */
void extractSpeedAndDir(String request){
  int posSpeed = request.indexOf("speed=");
  int posDir = request.indexOf("dir=");
  if (posSpeed != -1)
  {
    String speedString = request.substring(posSpeed+6, request.length());
    cmdSpeed =  speedString.toInt();
  }
  if (posDir != -1)
  {
    String dirString = request.substring(posDir+4, request.length());
    cmdDir =  dirString.toInt();
  }

  desiredSpeedM1 = cmdSpeed + cmdDir;
  desiredSpeedM2 = cmdSpeed - cmdDir;  
}

/*
 * Match the request, i.e. extract speed for both motors,e.g. motor?m1=-10&m2=20
 */
void extractMotorSpeeds(String request){
  int posM1 = request.indexOf("m1=");
  int posM2 = request.indexOf("m2=");
  if (posM1 != -1)
  {
    String speedM1 = request.substring(posM1+3, request.length());
    desiredSpeedM1 =  speedM1.toInt();
  }
  if (posM2 != -1)
  {
    String speedM1 = request.substring(posM2+3, request.length());
    desiredSpeedM2 =  speedM1.toInt();
  }
}

void adjustMotorSpeeds(){
  if (speedM1 < desiredSpeedM1)     speedM1++;
  if (speedM1 > desiredSpeedM1)     speedM1--;
  if (speedM2 < desiredSpeedM2)     speedM2++;
  if (speedM2 > desiredSpeedM2)     speedM2--;
  md.setSpeeds(speedM1, speedM2);
}


void connectWiFi()
{
  if (connectWIFI(WIFI_SSID1, WIFI_PASS1)) return;
  if (connectWIFI(WIFI_SSID2, WIFI_PASS2)) return;
  if (connectWIFI(WIFI_SSID3, WIFI_PASS3)) return;  
}


boolean connectWIFI(const char* ssid, const char* passwd)
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, passwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());
    return true;   
  }
 
  return false;
}


