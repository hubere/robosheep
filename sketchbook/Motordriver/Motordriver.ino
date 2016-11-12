


#include "DualMC33926MotorShield.h"

DualMC33926MotorShield md;

const int ledPin = 13;
int speedM1 = 0;
int speedM2 = 0;

void setup()
{
 pinMode(ledPin, OUTPUT);
 Serial.begin(9600);

  Serial.println("Dual MC33926 Motor Shield");
  md.init();
}

void loop()
{
  if (Serial.available()){
    int cmdSpeed = Serial.parseInt();
    int cmdDir = Serial.parseInt();
  
    Serial.print("received speed: ");
    Serial.print(cmdSpeed);
    Serial.print(" dir: ");
    Serial.println(cmdDir);

    adjustSpeedM1(cmdSpeed + cmdDir);
    adjustSpeedM2(cmdSpeed - cmdDir);
    
    Serial.print("M1: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print(" M2: ");
    Serial.println(md.getM2CurrentMilliamps());
  
  }
 delay(1000);
}

void adjustSpeedM1(int newSpeed){
  Serial.print("M1 ");
  Serial.println(newSpeed);
  if (newSpeed < speedM1){
     decelerate("M1", speedM1, newSpeed*4); 
  }else{
     accelerate("M1", speedM1, newSpeed*4); 
  }
  speedM1 = newSpeed; 
}

void adjustSpeedM2(int newSpeed){
  Serial.print("M2 ");
  Serial.println(newSpeed);
  if (newSpeed < speedM2){
     decelerate("M2", speedM2, newSpeed*4);
  }else{
     accelerate("M2", speedM2, newSpeed*4); 
  }
  speedM2 = newSpeed; 
}

void accelerate(String motor, int from, int to){  
  for (int i = from; i < to; i++)
  {
    if (motor.equals("M1")){
      md.setM1Speed(i);
    }else{
      md.setM2Speed(i);
    }
    delay(2);
  }
}

void decelerate(String motor, int from, int to){  
  for (int i = from; i > to; i--)
  {
    if (motor.equals("M1")){
      md.setM1Speed(i);
    }else{
      md.setM2Speed(i);
    }
    delay(2);
  }
}


