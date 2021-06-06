#ifndef WEB_SERVER_h
#define WEB_SERVER_h

#include <ESP8266WiFi.h>


#define RESP_STATUS_REQUESTED      0
#define RESP_COMMAND_WAS_ISSUED    1


WiFiServer server(80);

class WebServer
{
  private:
    
  public:  


    WebServer(){      
    }

    void init(){
      Serial.println("\nStarting Webserver");
      server.begin();
      delay(10);
    }

    /*
     * prepare a web page to be send to a client (web browser)
     */
    String prepareHtmlPage()
    {
      String htmlPage;
      htmlPage.reserve(1024);               // prevent ram fragmentation
      htmlPage = F("HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/html\r\n"
                   "Connection: close\r\n"  // the connection will be closed after completion of the response
                   // "Refresh: 5\r\n"         // refresh the page automatically every 5 sec
                   "\r\n");
      htmlPage += page1;
      htmlPage += F("\r\n");
      return htmlPage;
    }
    
    
    /*
     * 
     */
    void listenForIncomingRequests(){
      WiFiClient client = server.available();
      if (client)
      {
        handleClientRequest(client);
    
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
      } 
       
      // The client will actually be disconnected 
      // when the function returns and 'client' object is detroyed
    }  

    /*
     * Handle requests of web interface.
     * 
     */
    int handleClientRequest(WiFiClient client)
    {
      unsigned long currentMillis = millis();
      
      //
      // Wait until the client sends some data
      //
      // Serial.print("\n\nRequest from " + client.remoteIP().toString() + ": ");
    
      //
      // taken from https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
      //
      String request = "";
      String response = "";
      while (client.connected())
      {    
        // read line by line what the client (web browser) is requesting
        if (client.available())
        {
          String line = client.readStringUntil('\r');
          // Serial.println(line); // <- good for debugging purposes
          if (line.indexOf("GET") >= 0)
            request = line;
          
          // wait for end of client's request, that is marked with an empty line
          if (line.length() == 1 && line[0] == '\n')
          {        
            Serial.print(request);
            
            //
            // received valid request?
            //
            if (request.indexOf("/sheep/state") > 0){
              response = state.respondWithSheepState();
                
            }else if (request.indexOf("/sheep/set") > 0){
              extractSetParameter(request);
              response = state.respondWithSheepState();          
              
            }else if (request.indexOf("/sheep/move") > 0){
              extractSpeedAndDir(request);
              response = state.respondWithSheepState();          
              
            }else if (request.indexOf("/sheep/cutter/on") > 0){
              state.cutterOn();              
              response = state.respondWithSheepState();          
              
            }else if (request.indexOf("/sheep/cutter/off") > 0){
              state.cutterOff();              
              response = state.respondWithSheepState();          
              
            }else if (request.indexOf("/sheep/forward") > 0){
              int dist = extractDistance(request);
              state.setDesiredSpeeds(dist,dist);
              response = state.respondWithSheepState();          
              commandIssued();                    
    
            }else if (request.indexOf("/sheep/backward") > 0){
              int dist = extractDistance(request);
              state.setDesiredSpeeds(-dist,dist);
              response = state.respondWithSheepState();          
              commandIssued();                    
    
            }else if (request.indexOf("/sheep/left") > 0){
              int dist = extractDistance(request);
              state.setDesiredSpeeds(dist,-dist);
              response = state.respondWithSheepState();          
              commandIssued();                    
    
            }else if (request.indexOf("/sheep/right") > 0){
              int dist = extractDistance(request);
              state.setDesiredSpeeds(-dist,dist);
              commandIssued();                    
              response = state.respondWithSheepState();          
    
            }else if (request.indexOf("/motor") > 0){
              extractMotorSpeeds(request);    
              response = state.respondWithSheepState();
              
            }else{ 
              // serve index.html
              response = prepareHtmlPage();
              client.print(response);
              response ="index.html";
            }
            // client.flush(); // TODO FIXME HU what is that for?
            client.print(response);
            unsigned int responseTime = millis()-currentMillis;
            Serial.println("-> " + response + "(" + String(responseTime) + "ms)");   
            break;
          }
        }
      }
    
      while (client.available()) {
        // but first, let client finish its request
        // that's diplomatic compliance to protocols
        // (and otherwise some clients may complain, like curl)
        // (that is an example, prefer using a proper webserver library)
        client.read();
      }
      
    }

    /*
     * A command was issued. Reset alive timer.
     */
    void commandIssued(){
      state.lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
    }

    /*
     * Match the request, i.e. extract parameter to set and value
     */
    void extractSetParameter(String request){
      String maxSpeedString = extractParam(request, "maxSpeed");
      if (maxSpeedString != NULL)
      {
        state.maxSpeed =  maxSpeedString.toInt();
        Serial.println("Setting maxSpeed to "+ maxSpeedString);               
      }
      
      String errorM1String = extractParam(request, "errorM1");
      if (errorM1String != NULL)
      {
        state.setErrorM1(errorM1String.toInt());
        Serial.println("Setting errorM1 to "+ errorM1String);               
      }
      
      String errorM2String = extractParam(request, "errorM2");
      if (errorM2String != NULL)
      {
        state.setErrorM2(errorM2String.toInt());
        Serial.println("Setting errorM2 to "+ errorM2String);               
      }      
    }

    /*
     * Extract parameter @param as string from @request.
     * e.g. @param is given as 'maxSpeed' (Note: leave out the '=' sign)
     */
    String extractParam(String request, String param)
    {      
      int paramPos = request.indexOf(param);
      if (paramPos == -1) return (String)NULL;
      return request.substring(paramPos + param.length()+1, request.length()); // +1 is for '=' sign!      
    }

    
    /*
     * Match the request, i.e. extract speed and dir
     * and set desiredSpeedM1 and desiredSpeedM2
     */
    void extractSpeedAndDir(String request){
      String speedString = extractParam(request, "speed");
      String dirString = extractParam(request, "dir");
      
      if (speedString != NULL)        state.cmdSpeed =  speedString.toInt();
      if (dirString != NULL)          state.cmdDir =  dirString.toInt();
    
      int newDesireM1 = state.cmdSpeed + state.cmdDir;
      int newDesireM2 = state.cmdSpeed - state.cmdDir;  

      state.setDesiredSpeeds(newDesireM1,newDesireM2);
      commandIssued();                    
    }
    
    /*
     * Match the request, i.e. extract distance
     */
    int extractDistance(String request){
      String distString = extractParam(request, "dist");
      if (distString == NULL) return 0;
      return distString.toInt();      
    }
    
    /*
     * Match the request, i.e. extract speed for both motors,e.g. motor?m1=-10&m2=20
     */
    void extractMotorSpeeds(String request){
      int newDesireM1 = 0;
      int newDesireM2 = 0;
      String speedM1 = extractParam(request, "m1");
      String speedM2 = extractParam(request, "m2");
      if (speedM1 != NULL) newDesireM1 =  speedM1.toInt();
      if (speedM2 != NULL) newDesireM1 =  speedM2.toInt();
      
      state.setDesiredSpeeds(newDesireM1,newDesireM2);      
      commandIssued();                    
    }

    
};


WebServer webServer;


#endif
