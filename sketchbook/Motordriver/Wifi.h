

#include <ESP8266WiFi.h>


// WiFi connection configuration
#define WIFI_SSID1       "Hubflepuff Common Rooms"
#define WIFI_PASS1       "AccioWLAN"

#define WIFI_SSID2       "Buergernetz-WLAN"
#define WIFI_PASS2       "LeInternetUwant11"

#define WIFI_SSID3       "TP-Huabas"
#define WIFI_PASS3       "Welpenspiel"

#define WIFI_SSID4       "MartinRouterKing"
#define WIFI_PASS4       "Ihaveastream"

#define WIFI_SSID5       "Huabas"
#define WIFI_PASS5       "Welpenspiel"



// maximum number of tries during wifi connection setup
#define WIFI_MAX_TRIES  30

// delay in ms between checks during wifi connection setup
#define WIFI_DELAY_TRY  1000


/*
 * Try to connect to a certain WIFI
 */
boolean connectWIFI(const char* ssid, const char* passwd)
{
  Serial.println();
  Serial.println();
  Serial.print("Trying to connect to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, passwd);

  int tries = 0;
  while (tries < 30) {  // 30 tries are 30 seconds!
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("");
      Serial.print("WiFi connected to ");
      Serial.println(ssid);
      Serial.println(WiFi.localIP());
    
      // Measure Signal Strength (RSSI) of Wi-Fi connection
      unsigned long before = millis();    
      long rssi = WiFi.RSSI();  
      Serial.println("RSSI: " + String(rssi) + " (" + String(millis() - before) + ")");
          
      return true;         
    }
    delay(1000);  // give 'em 1 seconds...
    tries++;
  }

  Serial.println("... connection failed.");
  return false;

}



/*
 * Connect on of the known WIFIs
 */
void connectWiFi()
{
  while (true)
  {
//      if (connectWIFI(WIFI_SSID2, WIFI_PASS2)) return;
      if (connectWIFI(WIFI_SSID1, WIFI_PASS1)) return;
//      if (connectWIFI(WIFI_SSID3, WIFI_PASS3)) return;
//      if (connectWIFI(WIFI_SSID4, WIFI_PASS4)) return;
      if (connectWIFI(WIFI_SSID5, WIFI_PASS5)) return;
      delay(5000);
  }
}
