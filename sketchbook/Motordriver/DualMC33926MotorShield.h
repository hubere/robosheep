#ifndef DualMC33926MotorShield_h
#define DualMC33926MotorShield_h

// see https://iotbytes.wordpress.com/nodemcu-pinout/

#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15

#define SD2  9
#define SD3  10


class DualMC33926MotorShield
{
  public:  
  // Constructors ////////////////////////////////////////////////////////////////

  DualMC33926MotorShield()
  {
    //Pin map
    _M1DIR = D0;
    _M1PWM = D1;
    _M1FB =  D2; 

    _M2DIR = D5;
    _M2PWM = D6;
    _M2FB =  D7;
    
    _nD2 =   D4;
    _nSF =   D8;
  }


  // Public Methods //////////////////////////////////////////////////////////////
  void init()
  {
    // Define pinMode for the pins and set the frequency for timer1.
    pinMode(_M1DIR,OUTPUT);
    Serial.println("pinMode(_M1DIR (D0),OUTPUT)"); delay(10);    
    
    pinMode(_M1PWM,OUTPUT);
    Serial.println("pinMode(_M1PWM (D1),OUTPUT)"); delay(10);    
    
    pinMode(_M1FB,INPUT);
    Serial.println("pinMode(_M1FB  (D2),INPUT)"); delay(10);    
    
    pinMode(_M2DIR,OUTPUT);
    Serial.println("pinMode(_M2DIR (D5),OUTPUT)"); delay(10);    
    
    pinMode(_M2PWM,OUTPUT);
    Serial.println("pinMode(_M2PWM (D6),OUTPUT)"); delay(10);    
    
    pinMode(_M2FB,INPUT);
    Serial.println("pinMode(_M2FB  (D7),INPUT)"); delay(10);    
    
    pinMode(_nD2,OUTPUT);
    Serial.println("pinMode(_nD2 (D4),OUTPUT)"); delay(10);    
    
    pinMode(_nSF,INPUT);
    Serial.println("pinMode(_nSF (D8),OUTPUT)"); delay(10);    

    Serial.println("digitalWrite(_nD2,HIGH)"); delay(10);    
    
  }

// Set speed for motor 1, speed is a number betwenn -400 and 400
void setM1Speed(int speed)
{
  unsigned char reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;
  analogWrite(_M1PWM,speed * 51 / 800); // default to using analogWrite, mapping 400 to 255
  if (reverse)
    digitalWrite(_M1DIR,HIGH);
  else
    digitalWrite(_M1DIR,LOW);
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void setM2Speed(int speed)
{
  unsigned char reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;
  analogWrite(_M2PWM,speed * 51 / 800); // default to using analogWrite, mapping 400 to 255
  if (reverse)
    digitalWrite(_M2DIR,HIGH);
  else
    digitalWrite(_M2DIR,LOW);
}

// Set speed for motor 1 and 2
void setSpeeds(int m1Speed, int m2Speed)
{
  setM1Speed(m1Speed);
  setM2Speed(m2Speed);
}

// Return motor 1 current value in milliamps.
unsigned int getM1CurrentMilliamps()
{
  // 5V / 1024 ADC counts / 525 mV per A = 9 mA per count
  return analogRead(_M1FB) * 9;
}

// Return motor 2 current value in milliamps.
unsigned int getM2CurrentMilliamps()
{
  // 5V / 1024 ADC counts / 525 mV per A = 9 mA per count
  return analogRead(_M2FB) * 9;
}

// Return error status
unsigned char getFault()
{
  return !digitalRead(_nSF);
}
    
  private:
    unsigned char _nD2;
    unsigned char _M1DIR;
    unsigned char _M2DIR;
    unsigned char _M1PWM = 9;
    unsigned char _M2PWM = 10;
    unsigned char _nSF;
    unsigned char _M1FB;
    unsigned char _M2FB;
};





#endif
