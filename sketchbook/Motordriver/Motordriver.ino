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
#include "SheepState.h"
#include "BatteryPower.h"
#include "WebServer.h"
#include "compass.h"


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

/**
Label GPIO  Input     Output  Notes
D0  GPIO16  no int-   no PWM  HIGH at boot
            errupt    no I2C  used to wake up from deep sleep                                                                       
D1  GPIO5   OK        OK      often used as SCL (I2C)
D2  GPIO4   OK        OK      often used as SDA (I2C)
D3  GPIO0   pulled up OK      connected to FLASH button, boot fails if pulled LOW
D4  GPIO2   pulled up OK      HIGH at boot
                              connected to on-board LED, boot fails if pulled LOW
D5  GPIO14  OK        OK      SPI (SCLK)
D6  GPIO12  OK        OK      SPI (MISO)
D7  GPIO13  OK        OK      SPI (MOSI)
D8  GPIO15  pulled    OK      SPI (CS)
            to GND            Boot fails if pulled HIGH  
RX  GPIO3   OK        RX pin  HIGH at boot
TX  GPIO1   TX pin    OK      HIGH at boot
                              debug output at boot, boot fails if pulled LOW
A0  ADC0    AnalogIn  X 
 */

unsigned char CONNECTED_LED_PIN = D9_RX;  // yellow LED

/*
 * DON' USE THESE (used as default in Wire.h/QMC5883LCompass.h as SCL and SDA)
 * 
unsigned char SCL = D1;        
unsigned char SDA = D2;        
*/

/*
 * DON' USE THESE (see DualMC....h)
 * 
unsigned char _M1IN1  = D3;
unsigned char _M1IN2  = D4;
unsigned char _M2IN1  = D7;
unsigned char _M2IN2  = D8;
*/


/*
 * DON' USE THESE (see SheepState.h)
 * 
unsigned char PIN_CUTTER = D0;
unsigned char M1_STEP_PIN = D5;        // Digital pin to be read for M1 measurement.
unsigned char M2_STEP_PIN = D6;        // Digital pin to be read for M2 measurement.
*/ 

/*
 * DON' USE THESE (see BatteryPower.h)
 * 
const int ANALOG_PIN = A0;         // The only analog pin on the Thing
*/



unsigned char PIN_D10_NOT_USED = D10_TX;



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


void setup()
{
  Serial.begin(9600);
  delay(1000);
  Serial.println("\n\n\nStarting Robosheep.");
      
  // prepare GPIO
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(CONNECTED_LED_PIN, 1); delay(10);

  batteryPower.init();
  md.init();
  state.init();
  myWifi.init();
  webServer.init();
  compass.init();

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
  compass.readValues();
  stopOnLostConnection();                 // Stop if connection is lost, i.e. no commands issued anymore
  // adjustMotorSpeeds();                    // Adjust motor speeds towards desiredSpeed.
  myWifi.checkWifiConnection();
  webServer.handleClient();

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
  if (state.connectionLost(MAX_ALIVE_DELAY))
  {
    if (state.getDesiredSpeedM1() != 0 && state.getDesiredSpeedM2() != 0)
    {
      Serial.println("Warning! Lost connection to client. No command issued since ("+String(MAX_ALIVE_DELAY)+")");           
      state.stopMoving();          
    }
  }  
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
