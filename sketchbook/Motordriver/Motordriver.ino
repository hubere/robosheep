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


#include "Wifi.h"
#include "DualMC33926MotorShield.h"
#include "HTML.h"
#include "SheepState.h"
#include "BatteryPower.h"

//
// programm behavior
//

// When there was no command for MAX_ALIVE_DELAY ms, motors stop 
#define MAX_ALIVE_DELAY   5000    

// The loops delay. Each loop delays for at least LOOP_DELAY ms.
#define LOOP_DELAY         10 


/////////////////////
// Pin Definitions //
/////////////////////

unsigned char CONNECTED_LED_PIN = D0;  // yellow LED

/*
 * DON' USE THESE (see DualMC....h)
 * 
unsigned char _M1IN1  = D1;
unsigned char _M1IN2  = D2;
unsigned char _M2IN1  = D3;
unsigned char _M2IN2  = D4;
*/


/*
 * DON' USE THESE (see SheepState.h)
 * 
unsigned char M1_STEP_PIN = D5;        // Digital pin to be read for M1 measurement.
unsigned char M2_STEP_PIN = D6;        // Digital pin to be read for M2 measurement.
*/ 

/*
 * DON' USE THESE (see BatteryPower.h)
 * 
const int ANALOG_PIN = A0;         // The only analog pin on the Thing
*/
   
unsigned char PIN_D7_NOT_USED = D7;
unsigned char PIN_D8_NOT_USED = D8;
unsigned char PIN_D9_NOT_USED = D9;
unsigned char PIN_D10_NOT_USED = D10;


//
// global variables
// 
int loopCount = 0;
int bigLoopCount = 0;
unsigned long endOfLastLoopInMillis = millis();
unsigned long lastCommandTimestamp = millis();
int led_state = 0;  // last state set for led

//
// global objects
// 
WiFiServer server(80);
DualMC33926MotorShield md;


void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println("\n\nInitializing Dual MC33926 Motor Shield");
  md.init();
  delay(10);

  state.init();

  // prepare GPIO
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(CONNECTED_LED_PIN, 1); delay(10);

  // prepare battery power measurement
  pinMode(ANALOG_PIN, INPUT);

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
 * fast loop (about 800 executions in 10ms => 80MHz)
 * 
 */
void loop()
{
  unsigned long currentMillis = millis();
  loopCount++;

  if (timeForBigLoop(currentMillis - endOfLastLoopInMillis)) bigLoop();
}


/*
 * time for big loop!
 * we get here all LOOP_DELAY ms
 */
void bigLoop(){
  if (loopCount < 1){
    Serial.println("Warning! loopCount < 1 (" + String(loopCount) + "): ");       
  }    
  
  loopCount=0;  // <- they are usually about 800 once we get here
  bigLoopCount++;  

  stopOnLostConnection(millis() - lastCommandTimestamp); // Stop if connection is lost, i.e. no commands issued anymore
  toggleLED(bigLoopCount);  // indicate operation
  measureBatteryPower();    // battery power  
  adjustMotorSpeeds();      // Adjust motor speeds towards desiredSpeed.
  checkWifiConnection();
  listenForIncomingRequests();

  endOfLastLoopInMillis = millis();    
}



// --------- helper functions ---------

/*
 * 
 */
void listenForIncomingRequests(){
  WiFiClient client = server.available();
  if (client)
  {
    handleClientRequest(client);

    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } 
   
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


/*
 * check if WIFI is still connected and update rssi
 */
void checkWifiConnection(){
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi connection lost!");       
    connectWiFi();
  }
  state.rssi = WiFi.RSSI();  
}

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
  state.losingConnection = (MAX_ALIVE_DELAY - timeSinceLastCommand); //  / MAX_ALIVE_DELAY * 100;
  if (timeSinceLastCommand > MAX_ALIVE_DELAY)
  {
    state.desiredSpeedM1 = 0;
    state.desiredSpeedM2 = 0;
    //Serial.println("Warning! Lost connection to client. timeSinceLastCommand ("+String(timeSinceLastCommand)+") > MAX_ALIVE_DELAY ("+String(MAX_ALIVE_DELAY)+")");       
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
  // Serial.print("\n\nRequest from " + client.remoteIP().toString() + ": ");

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
          response = state.respondWithSheepState();
            
        }else if (request.indexOf("/sheep/set") > 0){
          extractSetParameter(request);
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          
        }else if (request.indexOf("/sheep/move") > 0){
          extractSpeedAndDir(request);
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          
        }else if (request.indexOf("/sheep/forward") > 0){
          int dist = extractDistance(request);
          state.desiredSpeedM1 = dist; 
          state.desiredSpeedM2 = dist;           
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          Serial.println("Setting desiredSpeed to ("+String(state.desiredSpeedM1)+"/"+String(state.desiredSpeedM2)+")");       

        }else if (request.indexOf("/sheep/backward") > 0){
          int dist = extractDistance(request);
          state.desiredSpeedM1 = -dist; 
          state.desiredSpeedM2 = -dist;           
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          Serial.println("Setting desiredSpeed to ("+String(state.desiredSpeedM1)+"/"+String(state.desiredSpeedM2)+")");       

        }else if (request.indexOf("/sheep/left") > 0){
          int dist = extractDistance(request);
          state.desiredSpeedM1 = dist; 
          state.desiredSpeedM2 = -dist;           
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          Serial.println("Setting desiredSpeed to ("+String(state.desiredSpeedM1)+"/"+String(state.desiredSpeedM2)+")");       

        }else if (request.indexOf("/sheep/right") > 0){
          int dist = extractDistance(request);
          state.desiredSpeedM1 = -dist; 
          state.desiredSpeedM2 = dist;           
          response = state.respondWithSheepState();          
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          Serial.println("Setting desiredSpeed to ("+String(state.desiredSpeedM1)+"/"+String(state.desiredSpeedM2)+")");       

          
        }else if (request.indexOf("/motor") > 0){
          extractMotorSpeeds(request);    
          response = state.respondWithSheepState();
          lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
          
        }else{ 
          // serve index.html
          response = prepareHtmlPage();
          client.print(response);
          response ="index.html";
        }
        // client.flush(); // TODO FIXME HU what is that for?
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
 * Match the request, i.e. extract parameter to set and value
 */
void extractSetParameter(String request){
  int posMaxSpeed = request.indexOf("maxSpeed=");
  if (posMaxSpeed != -1)
  {
    String maxSpeedString = request.substring(posMaxSpeed+9, request.length());
    state.maxSpeed =  maxSpeedString.toInt();
    Serial.println("Setting maxSpeed to "+String(state.maxSpeed));       
  }
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
    state.cmdSpeed =  speedString.toInt();
  }
  if (posDir != -1)
  {
    String dirString = request.substring(posDir+4, request.length());
    state.cmdDir =  dirString.toInt();
  }

  int newDesireM1 = state.cmdSpeed + state.cmdDir;
  int newDesireM2 = state.cmdSpeed - state.cmdDir;  
  state.desiredSpeedM1 += newDesireM1; 
  state.desiredSpeedM2 += newDesireM2; 
  Serial.println("Setting desiredSpeed to ("+String(state.desiredSpeedM1)+"/"+String(state.desiredSpeedM2)+")");       
}

/*
 * Match the request, i.e. extract distance
 */
int extractDistance(String request){
  int posDist = request.indexOf("dist=");
  if (posDist != -1)
  {
    String distString = request.substring(posDist+5, request.length());
    return distString.toInt();
  }
  return 0;
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
    state.desiredSpeedM1 =  speedM1.toInt();
  }
  if (posM2 != -1)
  {
    String speedM1 = request.substring(posM2+3, request.length());
    state.desiredSpeedM2 =  speedM1.toInt();
  }
}

/*
 * Bring motor speeds towards desiredSpeeds by steps of 1
 * and send speedMs to I/O
 */
void adjustMotorSpeeds(){
  if (state.speedM1 < state.desiredSpeedM1)     state.increaseM1();
  if (state.speedM1 > state.desiredSpeedM1)     state.decreaseM1();
  if (state.speedM2 < state.desiredSpeedM2)     state.increaseM2();
  if (state.speedM2 > state.desiredSpeedM2)     state.decreaseM2();

  int speedM1 = state.speedM1;
  int speedM2 = state.speedM2;
  int minSpeed = 10;
  
  // pay respect to nonkongruent speeds
  speedM1 -= state.diffDesired();
  speedM2 += state.diffDesired();
     
  //  ensure a minimum speed
  if (speedM1 > 0 && speedM1 <  minSpeed) speedM1 =  minSpeed;
  if (speedM1 < 0 && speedM1 > -minSpeed) speedM1 = -minSpeed;
  if (speedM2 > 0 && speedM2 <  minSpeed) speedM2 =  minSpeed;
  if (speedM2 < 0 && speedM2 > -minSpeed) speedM2 = -minSpeed;

  md.setSpeeds(speedM1, speedM2);

  Serial.print(".");
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
