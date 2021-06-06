#ifndef WIFI_h
#define WIFI_h


#include <ESP8266WiFi.h>
#include "SheepState.h"


// WiFi connection configuration
#define WIFI_SSID1       "Hubflepuff Common Rooms"
#define WIFI_PASS1       "AccioWLAN"

#define WIFI_SSID2       "Buergernetz-WLAN"
#define WIFI_PASS2       "LeInternetUwant11"

#define WIFI_SSID3       "TP-Huabas"
#define WIFI_PASS3       "Welpenspiel"

#define WIFI_SSID4       "Winternet is comming"
#define WIFI_PASS4       "YouKnowNothing"

#define WIFI_SSID5       "Huabas"
#define WIFI_PASS5       "Welpenspiel"



// maximum number of tries during wifi connection setup
#define WIFI_MAX_TRIES  100

// delay in ms between checks during wifi connection setup
#define WIFI_DELAY_TRY  1000



class MyWifi
{
  private:


  public:  


    MyWifi(){      
    }

    /*
     * Connect on of the known WIFIs
     */
    void init()
    {
      WiFi.mode(WIFI_STA);
      Serial.println("\nInitializing Wifi");
      while (true)
      {
    //      if (connectWIFI(WIFI_SSID2, WIFI_PASS2)) return;
          if (connectWIFI(WIFI_SSID1, WIFI_PASS1)) return;
    //      if (connectWIFI(WIFI_SSID3, WIFI_PASS3)) return;
    //      if (connectWIFI(WIFI_SSID4, WIFI_PASS4)) return;
    //      if (connectWIFI(WIFI_SSID5, WIFI_PASS5)) return;
          delay(1000);
      }
      delay(10);
      
    }    
    
    /*
     * Try to connect to a certain WIFI
     */
    boolean connectWIFI(const char* ssid, const char* passwd)
    {
      Serial.println();
      Serial.println();
      Serial.print("Trying to connect to '");
      Serial.print(ssid);
      Serial.println("'");
      
      WiFi.begin(ssid, passwd);
     
      int tries = 0;
      while (tries < WIFI_MAX_TRIES) {  // 30 tries are 30 seconds!
        if (WiFi.status() == WL_CONNECTED){
          Serial.println("");
          Serial.print("WiFi connected to ");
          Serial.println(ssid);
          Serial.println(WiFi.localIP());
        
          // Measure Signal Strength (RSSI) of Wi-Fi connection
          unsigned long before = millis();    
          Serial.println("RSSI: " + String(WiFi.RSSI()) + " (" + String(millis() - before) + ")");
              
          return true;         
        }
        Serial.print("("+ String(tries) +") ");
        // Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");

        switch (WiFi.status()) {        
          case 0 : Serial.println("WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses"); break;          
          case 1 : Serial.println("WL_NO_SSID_AVAILin case configured SSID cannot be reached"); break;          
          case 3 : Serial.println("WL_CONNECTED after successful connection is established"); break;          
          case 4 : Serial.println("WL_CONNECT_FAILED if connection failed"); break;          
          case 6 : Serial.println("WL_CONNECT_WRONG_PASSWORD if password is incorrect"); break;          
          case 7 : Serial.println("WL_DISCONNECTED if module is not configured in station mode"); break;
        }
        
        delay(1000);  // give 'em 1 seconds...
        tries++;
      }
    
      Serial.println("... connection failed.");
      return false;
    
    };

    /*
     * check if WIFI is still connected and update rssi
     */
    void checkWifiConnection(){
      if (WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi connection lost!");       
        init();
      }
      state.rssi = WiFi.RSSI();  
    }
    
};


MyWifi myWifi;









#endif
