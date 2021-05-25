#ifndef BATTERY_POWER_h
#define BATTERY_POWER_h


//
// battery power measurement
// see (https://www.electroschematics.com/arduino-digital-voltmeter/)
// 
// int analogInput = 0; -> see ANALOG_PIN
class BatteryPower
{
  private:
    float vout = 0.0;
    float vin = 0.0;
    float R1 = 100000.0; // resistance of R1 (100K) -see text!
    float R2 = 10000.0; // resistance of R2 (10K) - see text!
    int value = 0;
    int batteryPowerMeasurements = 0;
    long summedBatteryPowerValue = 0;
    
    const int ANALOG_PIN = A0;         // The only analog pin on the Thing
    
    
  public:  

    BatteryPower(){      
    }

    void init(){
      Serial.println("\nInitializing battery power.");
      // prepare battery power measurement
      pinMode(ANALOG_PIN, INPUT);
      delay(10);
    };


    /*
     * Measure battery power
     * (see https://www.engineersgarage.com/esp8266/nodemcu-battery-voltage-monitor/)
     * (see https://www.electroschematics.com/arduino-digital-voltmeter/)
     */
    void measureBatteryPower()
    {
      const int POWER_READINGS_COUNT = 10;
      
      summedBatteryPowerValue += analogRead(ANALOG_PIN);    
      batteryPowerMeasurements++;
      if (batteryPowerMeasurements >= POWER_READINGS_COUNT)
      {
        value = summedBatteryPowerValue / POWER_READINGS_COUNT;
        vout = (value * 3.3) / 1024.0; // Convert voltage to 5V factor
        vout = (value * 3.1) / 1024.0; // 2.9 was empeeric factor!!!! No clue why!?!
        vin = vout / (R2/(R1+R2)); 
        if (vin<0.09) vin=0.0;//statement to quash undesired reading !
        state.batteryPower = vin*10;
        batteryPowerMeasurements = 0;
        summedBatteryPowerValue = 0;
      }
    };    
  
};


BatteryPower batteryPower;







#endif
