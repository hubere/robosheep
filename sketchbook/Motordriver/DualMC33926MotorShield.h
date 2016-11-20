#ifndef DualMC33926MotorShield_h
#define DualMC33926MotorShield_h

#include "NodeMCU.h"


class DualMC33926MotorShield
{
  private:
    unsigned char _M1DIR  = D0;
    unsigned char _M1PWM  = D1;
    unsigned char _M2DIR  = D5;
    unsigned char _M2PWM  = D6;
    unsigned char _nD2    = D4;
    unsigned char _nSF    = D8;

  public:  
  // Constructors ////////////////////////////////////////////////////////////////

  DualMC33926MotorShield(){
  }


  // Public Methods //////////////////////////////////////////////////////////////
  void init(){
    
    pinMode(_M1DIR,OUTPUT);
    Serial.println("pinMode(_M1DIR (D0),OUTPUT)"); delay(10);    
    
    pinMode(_M1PWM,OUTPUT);
    Serial.println("pinMode(_M1PWM (D1),OUTPUT)"); delay(10);    
    
    pinMode(_M2DIR,OUTPUT);
    Serial.println("pinMode(_M2DIR (D5),OUTPUT)"); delay(10);    
    
    pinMode(_M2PWM,OUTPUT);
    Serial.println("pinMode(_M2PWM (D6),OUTPUT)"); delay(10);    
    
    pinMode(_nD2,OUTPUT);
    Serial.println("pinMode(_nD2 (D4),OUTPUT)"); delay(10);    
    
    pinMode(_nSF,INPUT);
    Serial.println("pinMode(_nSF (D8),OUTPUT)"); delay(10);    

    digitalWrite(_nD2,HIGH);
    Serial.println("digitalWrite(_nD2,HIGH)"); delay(10);    
  }

  void setSpeed(unsigned char pwmPin, unsigned char dirPin, int speed)
  {
    unsigned char dir = LOW;
    if (speed < 0)
    {
      speed = -speed;  // Make speed a positive quantity
      dir = HIGH;  // Preserve the direction
    }
    digitalWrite(dirPin, dir);

    if (speed > 100) speed = 100;
    analogWrite(pwmPin, speed * PWMRANGE / 100); // default to using analogWrite, mapping 400 to PWMRANGE (1023)
  }

  //
  // Set speed for motor 1 and 2
  //
  void setSpeeds(int m1Speed, int m2Speed)
  {
    setSpeed(_M1PWM, _M1DIR, m1Speed);
    setSpeed(_M2PWM, _M2DIR, m2Speed);  
  }

  //
  // Return error status
  //
  unsigned char getFault()
  {
    return !digitalRead(_nSF);
  }
    
};





#endif
