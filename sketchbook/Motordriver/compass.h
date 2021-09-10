#ifndef COMPASS_h
#define COMPASS_h

#include <QMC5883LCompass.h>






/**
 * These are the default SCL SDA pins used in QMC5883LCompass.cpp. It's hard to change them
 */
//unsigned char SCL = D1;        
//unsigned char SDA = D2;



class Compass
{
  private:
    QMC5883LCompass qmccompass;

  public:  


    Compass(){      
    }

    void init()
    {
      qmccompass.init();
    }

    void readValues()
    {
      qmccompass.read();      
      state.setCompassReadings(qmccompass.getAzimuth(), qmccompass.getX(), qmccompass.getY(), qmccompass.getZ());
    }

    int getHeading(){
      qmccompass.read();
      return qmccompass.getAzimuth();
    }    

  private:
    
    
};

Compass compass;



#endif
