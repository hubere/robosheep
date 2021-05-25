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
#include "WebServer.h"


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
int led_state = 0;  // last state set for led

//
// global objects
// 
DualMC33926MotorShield md;


void setup()
{
  Serial.begin(115200);
  delay(10);

  // prepare GPIO
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(CONNECTED_LED_PIN, 1); delay(10);

  batteryPower.init();
  md.init();
  state.init();
  myWifi.init();
  webServer.init();

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

  toggleLED(bigLoopCount);                // indicate operation
  batteryPower.measureBatteryPower();     // battery power  
  stopOnLostConnection();                 // Stop if connection is lost, i.e. no commands issued anymore
  adjustMotorSpeeds();                    // Adjust motor speeds towards desiredSpeed.
  myWifi.checkWifiConnection();
  webServer.listenForIncomingRequests();

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
void stopOnLostConnection()
{
  unsigned long timeSinceLastCommand = millis() - state.lastCommandTimestamp;
  state.losingConnection = (MAX_ALIVE_DELAY - timeSinceLastCommand); //  / MAX_ALIVE_DELAY * 100;
  if (timeSinceLastCommand > MAX_ALIVE_DELAY)
  {
    state.desiredSpeedM1 = 0;
    state.desiredSpeedM2 = 0;
    //Serial.println("Warning! Lost connection to client. timeSinceLastCommand ("+String(timeSinceLastCommand)+") > MAX_ALIVE_DELAY ("+String(MAX_ALIVE_DELAY)+")");       
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
