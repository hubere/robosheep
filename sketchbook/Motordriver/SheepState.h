/*
 * This file handles state of sheep / motors
 */

#ifndef SHEEP_STATE_h
#define SHEEP_STATE_h 

#include <ArduinoJson.h>
#include "DualMC33926MotorShield.h"

// forward declarations
void ISRFuncM1();
void ISRFuncM2();


int sign(int x)
{
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;  
}


class SheepState
{
  
  private:
    unsigned char M1_STEP_PIN = D5;        // Digital pin to be read for M1 measurement.
    unsigned char M2_STEP_PIN = D6;        // Digital pin to be read for M2 measurement.
    unsigned char PIN_CUTTER = D7;         // start / stop cutter 

    const int speedIncrease = 10; 
    const int minSpeed = 10; 

    int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
    int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)

    int errorM1 = 1;            // empiric value
    int errorM2 = 0;

  public:  
    long rssi = 0;  
    int maxSpeed = 25;  // max speed of a motor
    int speedM1 = 0;    // actual speed of motor 1 (PWM)
    int speedM2 = 0;    // actual speed of motor 2 (PWM)
    long posM1 = 0;
    long posM2 = 0;
    int cmdSpeed = 0;          // desired speed 
    int cmdDir = 0;            // desired dir
    int batteryPower = 0;      // power of battery in %
    int losingConnection = 0;  // timer for lost connection / no new commands from client.
    bool isCutterOn = false;
    unsigned long lastCommandTimestamp; // timestamp of last command 

    long posM1_onLastCommand = 0;
    long posM2_onLastCommand = 0;



    
    // Constructors ////////////////////////////////////////////////////////////////  
    SheepState(){
    }

    //
    // getter and setter
    //
    int getDesiredSpeedM1(){ return desiredSpeedM1; };
    int getDesiredSpeedM2(){ return desiredSpeedM2; };
    void setBatteryPower(int power){
      batteryPower = power;
    }

    int setErrorM1(int error){ errorM1 = error; };
    int setErrorM2(int error){ errorM2 = error; };


    //
    // methods
    //
    void init() {
        Serial.println();
        Serial.println("Initializing SheepState");
        
        // prepare interrupts
        pinMode(M1_STEP_PIN,INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(M1_STEP_PIN), ISRFuncM1, RISING);
        Serial.println("pinMode(M1_STEP_PIN (D5),INPUT_PULLUP)"); delay(10);          
        
        pinMode(M2_STEP_PIN,INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(M2_STEP_PIN), ISRFuncM2, RISING);
        Serial.println("pinMode(M2_STEP_PIN (D6),INPUT_PULLUP)"); delay(10);        

        pinMode(PIN_CUTTER, OUTPUT);
        digitalWrite(PIN_CUTTER, 1); delay(10);
        Serial.println("pinMode(PIN_CUTTER (D7),OUTPUT)"); delay(10);              
                 
    }

    boolean connectionLost(int maxAliveDelay){
      unsigned long timeSinceLastCommand = millis() - lastCommandTimestamp;
      losingConnection = (maxAliveDelay - timeSinceLastCommand); //  / MAX_ALIVE_DELAY * 100;
      return (timeSinceLastCommand > maxAliveDelay);
    }

    void stopMoving()
    {
      setDesiredSpeeds(0,0);    
    }

    void setDesiredSpeeds(int m1, int m2){

      Serial.println();
      Serial.println("setDesiredSpeeds(" + String(m1) + ", " + String(m2) + ") "); 
      Serial.println("  onLastCommand (" + String(posM1_onLastCommand ) + ", " + String(posM2_onLastCommand ) + ") "); 
      Serial.println("  pos           (" + String(posM1               ) + ", " + String(posM2               ) + ") "); 
      Serial.println("  distanceTrav. (" + String(distanceTraveledM1()) + ", " + String(distanceTraveledM2()) + ") "); 

      // when last command was a straight run, calculate error
/*      
 *    if (desiredSpeedM1 == desiredSpeedM2){
        errorM1 = distanceTraveledM1() - distanceTraveledM2();
        errorM2 = distanceTraveledM2() - distanceTraveledM1();
        Serial.println("  error       (" + String(errorM1) + ", " + String(errorM2) + ") "); 
      }
*/      
                
      desiredSpeedM1 = m1;
      desiredSpeedM2 = m2;  

      if (m1 == 0 && m2 == 0)
      {
        speedM1 = 0;
        speedM2 = 0;        
      }
      else
      {
        
        //  ensure a minimum and maximum speed
        //      speedM1 = adjustToLimits(m1);
        //      speedM2 = adjustToLimits(m2);
          
        speedM1 = sign(m1) * (maxSpeed + errorM1);
        speedM2 = sign(m2) * (maxSpeed + errorM2);
        
        posM1_onLastCommand = posM1;
        posM2_onLastCommand = posM2;
  
      }
      md.setSpeeds(speedM1, speedM2);        
    }

    int adjustToLimits(int speedMX)
    {
      if (speedMX > 0){
        speedMX = max(speedMX,  minSpeed);
        speedMX = min(speedMX,  maxSpeed);
      }
      if (speedMX < 0){
        speedMX = min(speedMX, -minSpeed);
        speedMX = max(speedMX, -maxSpeed);
      }
      return speedMX;
    }

    long distanceTraveledM1(){      return posM1 - posM1_onLastCommand;    }
    long distanceTraveledM2(){      return posM2 - posM2_onLastCommand;    }

    void ISR1() {
      if (speedM1 > 0) posM1++; else posM1--;
      if (desiredSpeedM1 > 0 && distanceTraveledM1() >= desiredSpeedM1) stopMoving();
      if (desiredSpeedM1 < 0 && distanceTraveledM1() <= desiredSpeedM1) stopMoving();
      
      //Serial.println ("ISR1: " + respondWithSheepState());
      Serial.print ("#");
    }    

    void ISR2() {
      if (speedM2 > 0) posM2++; else posM2--;

      if (desiredSpeedM2 > 0 && distanceTraveledM2() >= desiredSpeedM2) stopMoving();
      if (desiredSpeedM2 < 0 && distanceTraveledM2() <= desiredSpeedM2) stopMoving();

      //Serial.println ("ISR2: " + respondWithSheepState());
      Serial.print ("*");
    }    

    void cutterOn(){
      digitalWrite(PIN_CUTTER, 0); delay(10);
      isCutterOn = true;
    }

    void cutterOff(){
      digitalWrite(PIN_CUTTER, 1); delay(10);
      isCutterOn = false;
    }

    /*
     * Build json from internal state
     */
    String respondWithSheepState(){

    /*
     *  this approach seems to have memory problems :-(
     * 
      StaticJsonDocument<1000> doc;
      doc["m1"] = speedM1;
      doc["m2"] = speedM2;
      doc["desiredSpeedM1"] = desiredSpeedM1;
      doc["desiredSpeedM2"] = desiredSpeedM2;
      doc["posM1"] = posM1;
      doc["posM2"] = posM2;
      doc["maxSpeed"] = maxSpeed;
      doc["losingConnection"] = losingConnection;
      doc["power"] = batteryPower;
      doc["rssi"] = rssi;

      String response;
      serializeJson(doc, response);

      // example of json string: char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"      
 
    */

      String response;
      response =  "{";
      response = response  + "\"m1\":"                + speedM1 +       ", \"m2\":"            + speedM2        +",";
      response = response  + "\"desiredSpeedM1\":"    + desiredSpeedM1 +", \"desiredSpeedM2\":"+ desiredSpeedM2 +",";
      response = response  + "\"posM1\":"             + posM1 +         ", \"posM2\":"         + posM2          +",";
      response = response  + "\"errorM1\":"           + errorM1 +       ", \"errorM2\":"       + errorM2        +",";
      response = response  + "\"dist1\":"       + distanceTraveledM1() +", \"dist2\":"       + distanceTraveledM2()        +",";
      response = response  + "\"maxSpeed\":"          + maxSpeed +",";
      response = response  + "\"isCutterOn\":"        + isCutterOn +",";
      response = response  + "\"power\":"             + batteryPower +",";
      response = response  + "\"rssi\":"              + rssi +",";
      response = response  + "\"losingConnection\":"  + losingConnection +",";      
      response = response  + "\"nocomma\":0";      
      response = response  + "}";
      
      return response; 
    }

};


SheepState state;

// Checks if motion was detected on M1 or M2
ICACHE_RAM_ATTR void ISRFuncM1() {  state.ISR1();};
ICACHE_RAM_ATTR void ISRFuncM2() {  state.ISR2();};



/*
 * Count motor steps.
 */
  // when motor is spinning with 12%, signal is low for about 3000 loops and high for about 1600 loops
  // when motor is spinning with 25%, signal is low for about 1300 loops and high for about 700 loops
  // when motor is spinning with 100%, signal is low for about 20-300 loops and high for about 20-170 loops
/*
  
void countMotorSteps(){

  int M1_signal = digitalRead(M1_STEP_PIN);
  if (M1_isHigh and !M1_signal){  // changed from high to low      
    if (speedM1 > 0) posM1++; else posM1--;
    Serial.println("posM1: " + String(posM1) + " " );             
  }
  M1_isHigh = M1_signal;
  
  int M2_signal = digitalRead(M2_STEP_PIN);
  if (M2_isHigh and !M2_signal){  // changed from high to low      
    if (speedM2 > 0) posM2++; else posM2--;
    Serial.println("posM2: " + String(posM2) + "  " );             
  }
  M2_isHigh = M2_signal;  
}
*/

#endif
