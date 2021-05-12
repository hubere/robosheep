/*
 * This file handles state of sheep / motors
 */

#include <ArduinoJson.h>

// forward declarations
void ISRFuncM1();
void ISRFuncM2();


class SheepState
{
  
  private:
    unsigned char M1_STEP_PIN = D2;        // Digital pin to be read for M1 measurement.
    unsigned char M2_STEP_PIN = D3;        // Digital pin to be read for M2 measurement.

    const int speedIncrease = 1; 
    const int minSpeed = 10; 


  public:  
    int speedM1 = 0;    // actual speed of motor 1 (PWM)
    int speedM2 = 0;    // actual speed of motor 2 (PWM)
    long posM1 = 0;
    long posM2 = 0;
    int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
    int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)
    int cmdSpeed = 0;          // desired speed 
    int cmdDir = 0;            // desired dir
    long rssi = 0;  
    int batteryPower = 0;      // power of battery in %
    int losingConnection = 0;  // timer for lost connection / no new commands from client.

    
    // Constructors ////////////////////////////////////////////////////////////////  
    SheepState(){
    }

    void init() {
        Serial.println();
        Serial.println("Initializing SheepState");
        
        // prepare interrupts
        pinMode(M1_STEP_PIN,INPUT_PULLUP);
//        pinMode(M1_STEP_PIN,INPUT);
        attachInterrupt(digitalPinToInterrupt(M1_STEP_PIN), ISRFuncM1, RISING);
        Serial.println("pinMode(M1_STEP_PIN (D7),INPUT_PULLUP)"); delay(10);          
        
        pinMode(M2_STEP_PIN,INPUT_PULLUP);
//        pinMode(M2_STEP_PIN,INPUT);        
        attachInterrupt(digitalPinToInterrupt(M2_STEP_PIN), ISRFuncM2, RISING);
        Serial.println("pinMode(M2_STEP_PIN (D8),INPUT_PULLUP)"); delay(10);                 
    }

    void ISR1() {
      if (speedM1 > 0) posM1++; else posM1--;
      if (speedM1 == 0){
        if (desiredSpeedM1 > 0) desiredSpeedM1--; 
        if (desiredSpeedM1 < 0) desiredSpeedM1++;
      };
      if (speedM1 > 0) desiredSpeedM1--; 
      if (speedM1 < 0) desiredSpeedM1++;            
      // Serial.println("posM1: " + String(posM1) + " desiredSpeedM1: " + String(desiredSpeedM1));      
      Serial.println ("ISR1: " + respondWithSheepState());
    }    

    void ISR2() {
      if (speedM2 > 0) posM2++; else posM2--;
      if (speedM2 == 0){
        if (desiredSpeedM2 > 0) desiredSpeedM2--;
        if (desiredSpeedM2 < 0) desiredSpeedM2++;
      };
      if (speedM2 > 0) desiredSpeedM2--; 
      if (speedM2 < 0) desiredSpeedM2++;            
      // Serial.println("posM2: " + String(posM2) + " desiredSpeedM2: " + String(desiredSpeedM2));      
      Serial.println ("ISR2: " + respondWithSheepState());
    }    

    void increaseM1(){      speedM1 += speedIncrease;    }
    void decreaseM1(){      speedM1 -= speedIncrease;    }
    void increaseM2(){      speedM2 += speedIncrease;    }
    void decreaseM2(){      speedM2 -= speedIncrease;    }

    /*
     * Build json from internal state
     */
    String respondWithSheepState(){
    
      StaticJsonDocument<200> doc;
      doc["m1"] = speedM1;
      doc["m2"] = speedM2;
      doc["desiredSpeedM1"] = desiredSpeedM1;
      doc["desiredSpeedM2"] = desiredSpeedM2;
      doc["posM1"] = posM1;
      doc["posM2"] = posM2;
      doc["losingConnection"] = losingConnection;
      doc["power"] = batteryPower;
      doc["rssi"] = rssi;
    
      String response;
      serializeJson(doc, response);
        
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
