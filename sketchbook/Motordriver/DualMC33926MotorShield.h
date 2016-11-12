#ifndef DualMC33926MotorShield_h
#define DualMC33926MotorShield_h

//#include <Arduino.h>

class DualMC33926MotorShield
{
  public:  
// Constructors ////////////////////////////////////////////////////////////////

DualMC33926MotorShield()
{
  //Pin map
  _nD2 = 4;
  _M1DIR = 7;
  _M2DIR = 8;
  _nSF = 12;
  _M1FB = 0; 
  _M2FB = 1;
}

DualMC33926MotorShield(unsigned char M1DIR, unsigned char M1PWM, unsigned char M1FB,
                                               unsigned char M2DIR, unsigned char M2PWM, unsigned char M2FB,
                                               unsigned char nD2, unsigned char nSF)
{
  //Pin map
  //PWM1 and PWM2 cannot be remapped because the library assumes PWM is on timer1
  _nD2 = nD2;
  _M1DIR = M1DIR;
  _M2DIR = M2DIR;
  _nSF = nSF;
  _M1FB = M1FB; 
  _M2FB = M2FB;
}

// Public Methods //////////////////////////////////////////////////////////////
void init()
{
// Define pinMode for the pins and set the frequency for timer1.

  pinMode(_M1DIR,OUTPUT);
  pinMode(_M1PWM,OUTPUT);
  pinMode(_M1FB,INPUT);
  pinMode(_M2DIR,OUTPUT);
  pinMode(_M2PWM,OUTPUT);
  pinMode(_M2FB,INPUT);
  pinMode(_nD2,OUTPUT);
  digitalWrite(_nD2,HIGH); // default to on
  pinMode(_nSF,INPUT);

  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  ICR1 = 400;
  #endif
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
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1A = speed;
  #else
  analogWrite(_M1PWM,speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
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
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1B = speed;
  #else
  analogWrite(_M2PWM,speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
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
    static const unsigned char _M1PWM = 9;
    static const unsigned char _M2PWM = 10;
    unsigned char _nSF;
    unsigned char _M1FB;
    unsigned char _M2FB;
};





#endif
