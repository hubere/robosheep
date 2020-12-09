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
#define LOOP_DELAY         10 


// WiFi connection configuration
#define WIFI_PASS1       "Welpenspiel"
#define WIFI_SSID1       "Huabas"
#define WIFI_PASS2       "LeInternetUwant11"
#define WIFI_SSID2       "Buergernetz-WLAN"
#define WIFI_PASS3       "Welpenspiel"
#define WIFI_SSID3       "TP-Huabas"
#define WIFI_PASS4       "Ihaveastream"
#define WIFI_SSID4       "MartinRouterKing"
// maximum number of tries during wifi connection setup
#define WIFI_MAX_TRIES  30
// delay in ms between checks during wifi connection setup
#define WIFI_DELAY_TRY  1000


/////////////////////
// Pin Definitions //
/////////////////////

/*
 * DON' USE THESE (see DualMC....h)
 * 
unsigned char _M1IN1  = D0;
unsigned char _M1IN2  = D1;
unsigned char _nD2    = D4;
unsigned char _M2IN1  = D5;
unsigned char _M2IN2  = D6;
unsigned char _nSF    = D8;
*/

const int RED_LED_PIN = D2;        // red LED
const int CONNECTED_LED_PIN = D3;  // yellow LED
const int DIGITAL_PIN = D7;        // Digital pin to be read for M1 measurement.
const int ANALOG_PIN = A0;         // The only analog pin on the Thing


//
// global variables
// 
int loopCount = 0;
int bigLoopCount = 0;
unsigned long endOfLastLoopInMillis = millis();
unsigned long lastCommandTimestamp = millis();
int speedM1 = 0;    // actual speed of motor 1 (PWM)
int speedM2 = 0;    // actual speed of motor 2 (PWM)
int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)
int cmdSpeed = 0;   // desired speed 
int cmdDir = 0;     // desired dir

int led_state = 0;  // last state set for led

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
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(CONNECTED_LED_PIN, 1);
  delay(10);
  digitalWrite(CONNECTED_LED_PIN, 0);

  Serial.println("\nInitializing Dual MC33926 Motor Shield");
  md.init();
  delay(10);

  Serial.println("\nInitializing Wifi");
  connectWiFi();
  delay(10);

  Serial.println("\nStarting Webserver");
  server.begin();
  delay(10);

  printUsage();

  endOfLastLoopInMillis = millis();  
}

/*
 * This is THE BIG LOOP
 * 
 * The loop is delayed to ensure each loop is run after LOOP_DELAY ms. This way
 * we ensure constant frequency for actions taken in the loop.
 * 
 * An alive check stops the engines if no command was issued for more than MAX_ALIVE_DELAY ms.
 * 
 * Adjust motor speeds towards desiredSpeed.
 * 
 * Listen for new commands...
 * 
 * 
 * Note:
 * With a Serial.println, one loop takes 3 to 4ms
 * Without a Serial.println, we do about 800 loopCounts in 10 ms
 * 
 * 
 */
void loop()
{
  unsigned long currentMillis = millis();

  //
  // fast loop (about 800 executions in 10ms => 80MHz)
  //
  loopCount++;

  //
  // end of fast loop
  //
  if (not timeForBigLoop(currentMillis - endOfLastLoopInMillis))
    return;
    
  //
  // time for big loop!
  // we get here all LOOP_DELAY ms
  //
  if (loopCount < 1){
    Serial.println("Warning! loopCount < 1 (" + String(loopCount) + "): ");       
  }  
  loopCount=0;  // <- they are usually about 800 once we get here
  bigLoopCount++;  
  endOfLastLoopInMillis = millis();  

  //
  // indicate operation
  //
  toggleLED(bigLoopCount);

  //
  // Stop if connection is lost, i.e. no commands issued anymore
  //
  stopOnLostConnection(currentMillis - lastCommandTimestamp);
    
  //
  // Adjust motor speeds towards desiredSpeed.
  //
  adjustMotorSpeeds();
 
  //
  // listen for incoming clients
  //
  WiFiClient client = server.available();  
  if (client)
  {
    handleClientRequest(client);

    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  }

  //
  // and of big loop
  //
  endOfLastLoopInMillis = millis();
  return;    
  
  // A command was issued, reset alive check timer
  lastCommandTimestamp = millis();
  
  //delay(1); // what is that for?!?!
  Serial.println("");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
  
  endOfLastLoopInMillis = millis();
}

// --------- helper functions ---------

/*
 * returns true if timeSinceLastLoop exceeds LOOP_DELAY.
 */
boolean timeForBigLoop(unsigned long timeSinceLastLoop)
{    
  return timeSinceLastLoop > LOOP_DELAY;
}


/*
 * toggle LED each 100ms (each 10th bigLoopCount)
 */
void toggleLED(int bigLoopCount)
{
  if (bigLoopCount % 10 == 0){
    if (led_state == 1){
      led_state = 0;
    }else{
      led_state = 1;
    }
    digitalWrite(CONNECTED_LED_PIN, led_state);   
  }  
}


/*
 * Alive check - if client not alive, stop engines
 */
void stopOnLostConnection(unsigned long timeSinceLastCommand)
{
  if (timeSinceLastCommand > MAX_ALIVE_DELAY)
  {
    desiredSpeedM1 = 0;
    desiredSpeedM2 = 0;
  }
}

/*
 * Handle requests von web interface.
 * 
 */
void handleClientRequest(WiFiClient client)
{
  unsigned long currentMillis = millis();
  
  //
  // Wait until the client sends some data
  //
  Serial.print("\n\nRequest from " + client.remoteIP().toString() + ": ");

  //
  // taken from https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
  //
  String request = "";
  String response = "";
  while (client.connected())
  {    
    // read line by line what the client (web browser) is requesting
    if (client.available())
    {
      String line = client.readStringUntil('\r');
      // Serial.println(line); // <- good for debugging purposes
      if (line.indexOf("GET") >= 0)
        request = line;
      
      // wait for end of client's request, that is marked with an empty line
      if (line.length() == 1 && line[0] == '\n')
      {        
        Serial.print(request);
        
        //
        // received valid request?
        //
        if (request.indexOf("/sheep/state") > 0){
          response = respondWithSheepState();
            
        }else if (request.indexOf("/sheep/move") > 0){
          extractSpeedAndDir(request);
          response = respondWithSheepState();
          
        }else if (request.indexOf("/motor") > 0){
          extractMotorSpeeds(request);    
          response = respondWithSheepState();
          
        }else{ 
          // serve index.html
          response = prepareHtmlPage();
          client.print(response);
          response ="index.html";
        }
        // client.flush(); // TODO FIXME HU what it that for?
        client.print(response);
        unsigned int responseTime = millis()-currentMillis;
        Serial.println("-> " + response + "(" + String(responseTime) + "ms)");   
        break;
      }
    }
  }

  while (client.available()) {
    // but first, let client finish its request
    // that's diplomatic compliance to protocols
    // (and otherwise some clients may complain, like curl)
    // (that is an example, prefer using a proper webserver library)
    client.read();
  }
  
}
  


/*
 * Build json from internal state
 */
String respondWithSheepState(){

  String response = "{";
  response += "\"m1\":\"";
  response += speedM1;
  response += "\", ";
  response += "\"m2\":\""; 
  response += speedM2;
  response += "\"";
  response += "}";
  return response; 
}


/*
 * Match the request, i.e. extract speed and dir
 * and set desiredSpeedM1 and desiredSpeedM2
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

/*
 * Bring motor speeds towards desiredSpeeds by steps of 1
 * and send speedMs to I/O
 */
void adjustMotorSpeeds(){
  if (speedM1 < desiredSpeedM1)     speedM1++;
  if (speedM1 > desiredSpeedM1)     speedM1--;
  if (speedM2 < desiredSpeedM2)     speedM2++;
  if (speedM2 > desiredSpeedM2)     speedM2--;
  md.setSpeeds(speedM1, speedM2);
}

/*
 * Connect to all known WIFIs
 */
void connectWiFi()
{
  if (connectWIFI(WIFI_SSID1, WIFI_PASS1)) return;
  if (connectWIFI(WIFI_SSID2, WIFI_PASS2)) return;
  if (connectWIFI(WIFI_SSID3, WIFI_PASS3)) return;  
  if (connectWIFI(WIFI_SSID4, WIFI_PASS4)) return;  
}

/*
 * Try to connect to a certain WIFI
 */
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
    Serial.println(WiFi.localIP());
    return true;   
  }
 
  return false;
}


/*
 * prepare a web page to be send to a client (web browser)
 */
String prepareHtmlPage()
{
  String htmlPage;
  htmlPage.reserve(1024);               // prevent ram fragmentation
  htmlPage = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"  // the connection will be closed after completion of the response
               // "Refresh: 5\r\n"         // refresh the page automatically every 5 sec
               "\r\n");
  htmlPage += page1;
  htmlPage += F("\r\n");
  return htmlPage;
}


void printUsage()
{
  //
  // print usage on serial monitor
  //
  Serial.println("Usage:");
  Serial.println("");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/sheep/state");
  Serial.println("");
  Serial.println("return sheep_state: {'m1': <speedM1>, 'm2' : <speedM2>} where");
  Serial.println("  <speedM1> : speed of motor 1 [-100, 100]");
  Serial.println("  <speedM2> : speed of motor 2 [-100, 100]");
  Serial.println("");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/sheep/move?speed=<speed>&dir=<dir>");
  Serial.println("");
  Serial.println("where");
  Serial.println("  <speed> : -100 to 100");
  Serial.println("  <dir>   : |speed| + |dir| < 100");
  Serial.println("");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/motor?m1=<speedM1>&m2=<speedM2>");
  Serial.println("");
  Serial.println("where");
  Serial.println("  <speedM1> : -100 to 100");
  Serial.println("  <speedM2> : -100 to 100");
  Serial.println("");  
}
