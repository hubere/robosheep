#ifndef DualMC33926MotorShield_h
#define DualMC33926MotorShield_h

#include "NodeMCU.h"



class DualMC33926MotorShield
{
  private:
    // unsigned char _nD2    = D0;  -> set _nD2 on Mottordrivershield to high!
    unsigned char _M1IN1  = D1;
    unsigned char _M1IN2  = D2;
    unsigned char _M2IN1  = D3;
    unsigned char _M2IN2  = D4;
    // unsigned char _nSF    = D8;  -> no posibility to read error state

    String LOGGER_NAME = "DualMC33926MotorShield";
    void log(String msg){        Serial.println(LOGGER_NAME + ": " + msg);    }

  public:  
  // Constructors ////////////////////////////////////////////////////////////////

    DualMC33926MotorShield(){
    }
  
  
    // Public Methods //////////////////////////////////////////////////////////////
    void init(){
      log("\n\nInitializing Dual MC33926 Motor Shield");
      
      pinMode(_M1IN1,OUTPUT);
      log("pinMode(_M1IN1 (D0),OUTPUT)"); delay(10);
      
      pinMode(_M1IN2,OUTPUT);
      log("pinMode(_M1IN2 (D1),OUTPUT)"); delay(10);
      
      pinMode(_M2IN1,OUTPUT);
      log("pinMode(_M2IN1 (D5),OUTPUT)"); delay(10);
      
      pinMode(_M2IN2,OUTPUT);
      log("pinMode(_M2IN2 (D6),OUTPUT)"); delay(10);
      
      //pinMode(_nD2,OUTPUT);
      //log("pinMode(_nD2 (D4),OUTPUT)"); delay(10);
      
      //pinMode(_nSF,INPUT);
      //log("pinMode(_nSF (D8),INPUT)"); delay(10);
  
      //digitalWrite(_nD2,HIGH);
      // log("digitalWrite(_nD2,HIGH)"); delay(10);
  
      delay(10);
  
    }
  
    //
    // Set speed for motor 1 and 2
    //
    void setSpeeds(int m1Speed, int m2Speed)
    {
      // TODO if huge speed is to ruckelig, make loop here!
      log("setSpeeds(" + String(m1Speed) +","+ String(m2Speed) + ")");
      setSpeed(_M1IN1, _M1IN2, m1Speed);
      setSpeed(_M2IN1, _M2IN2, m2Speed);  
    }


  private:

    void setSpeed(unsigned char pinForward, unsigned char pinBackward, int speed)
    {
      if (speed > 0){
        digitalWrite(pinBackward, LOW);
        setPosSpeed(pinForward, speed);
      }else{
        digitalWrite(pinForward, LOW);
        setPosSpeed(pinBackward, -speed);
      }
    }
  
    void setPosSpeed(unsigned char pin, int speed)
    {
      if (speed > 100) speed = 100;
      analogWrite(pin, speed * PWMRANGE / 100); // default to using analogWrite, mapping 400 to PWMRANGE (1023)
    }
  
  
    
};


DualMC33926MotorShield md;



#endif
