/*
 * This file handles state of sheep / motors
 */

#include <ArduinoJson.h>

//
// sheep state
//
int speedM1 = 0;    // actual speed of motor 1 (PWM)
int speedM2 = 0;    // actual speed of motor 2 (PWM)
long posM1 = 0;
long posM2 = 0;
int desiredSpeedM1 = 0;    // desired speed of motor 1 (PWM)
int desiredSpeedM2 = 0;    // desired speed of motor 2 (PWM)
int cmdSpeed = 0;   // desired speed 
int cmdDir = 0;     // desired dir
long rssi = 0;  
int batteryPower = 0;  // power of battery in %
int losingConnection = 0;  // timer for lost connection / no new commands from client.


//
// measurement on motors steps
//
boolean M1_isHigh = false;
boolean M2_isHigh = false;

const int M1_STEP_PIN = D7;        // Digital pin to be read for M1 measurement.
const int M2_STEP_PIN = D8;        // Digital pin to be read for M2. NOTE: same as _nSF (which is not used)



// Checks if motion was detected on M1
ICACHE_RAM_ATTR void detectsMovementM1() {
  if (speedM1 > 0) posM1++; else posM1--;
  Serial.println("posM1 " + String(posM1));
}

// Checks if motion was detected on M1
ICACHE_RAM_ATTR void detectsMovementM2() {
  if (speedM2 > 0) posM2++; else posM2--;
  Serial.println("posM2 " + String(posM2));
}


/*
 * Count motor steps.
 */
  // when motor is spinning with 12%, signal is low for about 3000 loops and high for about 1600 loops
  // when motor is spinning with 25%, signal is low for about 1300 loops and high for about 700 loops
  // when motor is spinning with 100%, signal is low for about 20-300 loops and high for about 20-170 loops
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
  
  
/*
  String response = "{";
  response += "\"m1\":\"" + speedM1 + "\", " + "\"m2\":\"" + speedM2 + "\", ";
  response += "\"desiredSpeedM1\":\"" + desiredSpeedM1 + "\", " + "\"desiredSpeedM2\":\"" + desiredSpeedM2 + "\", ";
  response += "\"posM1\":\"" + posM1 + "\", " + "\"posM2\":\"" + posM2 + "\", ";
  response += "\"losingConnection\":\"" + losingConnection + "\", ";
  response += "\"power\":\"" + batteryPower + "\", ";
  response += "\"rssi\":\"" + rssi + "\"";
  response += "}";
*/  
  return response; 
}
