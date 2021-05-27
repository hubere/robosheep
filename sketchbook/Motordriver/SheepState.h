/*
 * This file handles state of sheep / motors
 */

#ifndef SHEEP_STATE_h
#define SHEEP_STATE_h 

#include <ArduinoJson.h>

// forward declarations
void ISRFuncM1();
void ISRFuncM2();


class SheepState
{
  
  private:
    unsigned char M1_STEP_PIN = D5;        // Digital pin to be read for M1 measurement.
    unsigned char M2_STEP_PIN = D6;        // Digital pin to be read for M2 measurement.

    const int speedIncrease = 10; 
    const int minSpeed = 10; 

  public:  
    long rssi = 0;  
    int maxSpeed = 25;  // max speed of a motor
    int speedM1 = 0;    // actual speed of motor 1 (PWM)
    int speedM2 = 0;    // actual speed of motor 2 (PWM)
    long posM1 = 0;
    long posM2 = 0;
    int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
    int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)
    int cmdSpeed = 0;          // desired speed 
    int cmdDir = 0;            // desired dir
    int batteryPower = 0;      // power of battery in %
    int losingConnection = 0;  // timer for lost connection / no new commands from client.
    unsigned long lastCommandTimestamp; // timestamp of last command 



    
    // Constructors ////////////////////////////////////////////////////////////////  
    SheepState(){
    }

    void init() {
        Serial.println();
        Serial.println("Initializing SheepState");
        
        // prepare interrupts
        pinMode(M1_STEP_PIN,INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(M1_STEP_PIN), ISRFuncM1, RISING);
        Serial.println("pinMode(M1_STEP_PIN (D7),INPUT_PULLUP)"); delay(10);          
        
        pinMode(M2_STEP_PIN,INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(M2_STEP_PIN), ISRFuncM2, RISING);
        Serial.println("pinMode(M2_STEP_PIN (D8),INPUT_PULLUP)"); delay(10);                 
    }

    void setBatteryPower(int power){
      batteryPower = power;
    }

    boolean connectionLost(int maxAliveDelay){
      unsigned long timeSinceLastCommand = millis() - lastCommandTimestamp;
      losingConnection = (maxAliveDelay - timeSinceLastCommand); //  / MAX_ALIVE_DELAY * 100;
      return (timeSinceLastCommand > maxAliveDelay);
    }

    void setDesiredSpeeds(int m1, int m2){
      desiredSpeedM1 = m1;
      desiredSpeedM2 = m2;      
    }

    void ISR1() {
      if (speedM1 > 0) posM1++; else posM1--;
      if (speedM1 == 0){
        if (desiredSpeedM1 > 0) desiredSpeedM1--; 
        if (desiredSpeedM1 < 0) desiredSpeedM1++;
      };
      if (speedM1 > 0) desiredSpeedM1--; 
      if (speedM1 < 0) desiredSpeedM1++;            
      //Serial.println ("ISR1: " + respondWithSheepState());
      Serial.print ("#");

      // TODO FIXME HU adjust Motorspeeds
    }    

    void ISR2() {
      if (speedM2 > 0) posM2++; else posM2--;
      if (speedM2 == 0){
        if (desiredSpeedM2 > 0) desiredSpeedM2--;
        if (desiredSpeedM2 < 0) desiredSpeedM2++;
      };
      if (speedM2 > 0) desiredSpeedM2--; 
      if (speedM2 < 0) desiredSpeedM2++;            
      //Serial.println ("ISR2: " + respondWithSheepState());
      Serial.print ("*");

      // TODO FIXME HU adjust Motorspeeds

    }    

    void increaseM1(){ speedM1 += speedIncrease; speedM1 = min(speedM1,  maxSpeed);   }
    void decreaseM1(){ speedM1 -= speedIncrease; speedM1 = max(speedM1, -maxSpeed);   }
    void increaseM2(){ speedM2 += speedIncrease; speedM2 = min(speedM2,  maxSpeed);   }
    void decreaseM2(){ speedM2 -= speedIncrease; speedM2 = max(speedM2, -maxSpeed);   }

    int diffDesired(){ return desiredSpeedM1 - desiredSpeedM2; }

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
    */

      // example of json string: char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"      

      String response;
      response =  "{";
      response = response  + "\"m1\":"+ speedM1 +", \"m2\":"+ speedM2 +",";
      response = response  + "\"desiredSpeedM1\":"+ desiredSpeedM1 +", \"desiredSpeedM2\":"+ desiredSpeedM2 +",";
      response = response  + "\"posM1\":"+ posM1 +", \"posM2\":"+ posM2 +",";
      response = response  + "\"maxSpeed\":"+ maxSpeed +",";
      response = response  + "\"power\":"+ batteryPower +",";
      response = response  + "\"rssi\":"+ rssi +",";
      response = response  + "\"losingConnection\":"+ losingConnection +",";
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
