#ifndef WEB_SERVER_h
#define WEB_SERVER_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


#define RESP_STATUS_REQUESTED      0
#define RESP_COMMAND_WAS_ISSUED    1

ESP8266WebServer server(80);

const char * swaggerJSON = "{\"swagger\":\"2.0\",\"info\":{\"description\":\"This is the REST interface to the sheep.\",\"version\":\"1.0.0\",\"title\":\"Robosheep\"},\"host\":\"192.168.1.40\",\"tags\":[{\"name\":\"State\",\"description\":\"Getting state of sheep\"},{\"name\":\"Move\",\"description\":\"Make sheep move\"},{\"name\":\"Cutter\",\"description\":\"Make sheep eat\"}],\"paths\":{\"/sheep/state\":{\"get\":{\"tags\":[\"State\"],\"summary\":\"Endpoint for getting internal state of sheep\",\"description\":\"\",\"operationId\":\"getSheep\",\"responses\":{\"200\":{\"description\":\"Actual state of sheep\",\"schema\":{\"$ref\":\"#/definitions/sheepState\"}}}}},\"/sheep/set\":{\"get\":{\"tags\":[\"State\"],\"summary\":\"Endpoint for setting certain parameters of sheep\",\"description\":\"\",\"operationId\":\"setParameter\",\"parameters\":[{\"in\":\"query\",\"name\":\"maxSpeed\",\"type\":\"integer\",\"description\":\"The number of items to skip before starting to collect the result set\"},{\"in\":\"query\",\"name\":\"errorM1\",\"type\":\"integer\",\"description\":\"Error of motor M1. This value is added to output of current for M1\"},{\"in\":\"query\",\"name\":\"errorM2\",\"type\":\"integer\",\"description\":\"Error of motor M2. This value is added to output of current for M2\"}],\"responses\":{\"200\":{\"description\":\"Set paramerter of sheep\"}}}},\"/sheep/move\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep by setting speed and direction.\",\"description\":\"Description of Endpoint for moving sheep\",\"parameters\":[{\"in\":\"query\",\"name\":\"speed\",\"type\":\"integer\",\"description\":\"Speed to set for both motors in percentage.\"},{\"in\":\"query\",\"name\":\"dir\",\"type\":\"integer\",\"description\":\"Amount of difference of motors speed in percentage. dir=10 will increase M1 by 10% and decrease M2 by 10%\"}],\"responses\":{\"200\":{\"description\":\"Sheep is moving\"}}}},\"/sheep/motor\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep by setting speed and direction.\",\"description\":\"Description of Endpoint for moving sheep\",\"parameters\":[{\"in\":\"query\",\"name\":\"m1\",\"type\":\"integer\",\"description\":\"Speed to set to motor M1 in percentage.\"},{\"in\":\"query\",\"name\":\"m2\",\"type\":\"integer\",\"description\":\"Speed to set to motor M2 in percentage.\"}],\"responses\":{\"200\":{\"description\":\"Sheep is moving\"}}}},\"/sheep/forward\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep forward\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving forward\"}}}},\"/sheep/backward\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep backward\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving backward\"}}}},\"/sheep/left\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep left\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving left\"}}}},\"/sheep/right\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep right\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving right\"}}}},\"/cutter/on\":{\"get\":{\"tags\":[\"Cutter\"],\"summary\":\"Endpoint starting cutter\",\"description\":\"\",\"responses\":{\"200\":{\"description\":\"Cutter is running\"}}}},\"/cutter/off\":{\"get\":{\"tags\":[\"Cutter\"],\"summary\":\"Endpoint for stopping cutter\",\"description\":\"\",\"responses\":{\"200\":{\"description\":\"Cutter is shut down\"}}}}},\"definitions\":{\"sheepState\":{\"type\":\"object\",\"properties\":{\"m1\":{\"type\":\"integer\"},\"m2\":{\"type\":\"integer\"},\"desiredSpeedM1\":{\"type\":\"integer\"},\"desiredSpeedM2\":{\"type\":\"integer\"},\"posM1\":{\"type\":\"integer\"},\"posM2\":{\"type\":\"integer\"},\"errorM1\":{\"type\":\"integer\"},\"errorM2\":{\"type\":\"integer\"},\"maxSpeed\":{\"type\":\"integer\"},\"isCutterOn\":{\"type\":\"boolean\"},\"power\":{\"type\":\"integer\"},\"rssi\":{\"type\":\"integer\"},\"losingConnection\":{\"type\":\"integer\"}}}}}";
const char * swaggerUI = "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"> <meta http-equiv=\"x-ua-compatible\" content=\"IE=edge\"> <title>Swagger UI</title> <link href='https://cdnjs.cloudflare.com/ajax/libs/meyer-reset/2.0/reset.min.css' media='screen' rel='stylesheet' type='text/css'/> <link href='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/css/screen.css' media='screen' rel='stylesheet' type='text/css'/> <script>if (typeof Object.assign !='function'){(function (){Object.assign=function (target){'use strict'; if (target===undefined || target===null){throw new TypeError('Cannot convert undefined or null to object');}var output=Object(target); for (var index=1; index < arguments.length; index++){var source=arguments[index]; if (source !==undefined && source !==null){for (var nextKey in source){if (Object.prototype.hasOwnProperty.call(source, nextKey)){output[nextKey]=source[nextKey];}}}}return output;};})();}</script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/1.8.0/jquery-1.8.0.min.js' type='text/javascript'></script> <script>(function(b){b.fn.slideto=function(a){a=b.extend({slide_duration:\"slow\",highlight_duration:3E3,highlight:true,highlight_color:\"#FFFF99\"},a);return this.each(function(){obj=b(this);b(\"body\").animate({scrollTop:obj.offset().top},a.slide_duration,function(){a.highlight&&b.ui.version&&obj.effect(\"highlight\",{color:a.highlight_color},a.highlight_duration)})})}})(jQuery); </script> <script>jQuery.fn.wiggle=function(o){var d={speed:50,wiggles:3,travel:5,callback:null};var o=jQuery.extend(d,o);return this.each(function(){var cache=this;var wrap=jQuery(this).wrap('<div class=\"wiggle-wrap\"></div>').css(\"position\",\"relative\");var calls=0;for(i=1;i<=o.wiggles;i++){jQuery(this).animate({left:\"-=\"+o.travel},o.speed).animate({left:\"+=\"+o.travel*2},o.speed*2).animate({left:\"-=\"+o.travel},o.speed,function(){calls++;if(jQuery(cache).parent().hasClass('wiggle-wrap')){jQuery(cache).parent().replaceWith(cache);}if(calls==o.wiggles&&jQuery.isFunction(o.callback)){o.callback();}});}});}; </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery.ba-bbq/1.2.1/jquery.ba-bbq.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/handlebars.js/4.0.5/handlebars.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/lodash-compat/3.10.1/lodash.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/backbone.js/1.1.2/backbone-min.js' type='text/javascript'></script> <script>Backbone.View=(function(View){return View.extend({constructor: function(options){this.options=options ||{}; View.apply(this, arguments);}});})(Backbone.View); </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/swagger-ui.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/highlight.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/languages/json.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/json-editor/0.7.28/jsoneditor.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/marked/0.3.6/marked.min.js' type='text/javascript'></script> <script type=\"text/javascript\">$(function (){url=\"http://192.168.1.40/swagger.json\"; hljs.configure({highlightSizeThreshold: 5000}); window.swaggerUi=new SwaggerUi({url: url, dom_id: \"swagger-ui-container\", supportedSubmitMethods: ['get', 'post', 'put', 'delete', 'patch'],validatorUrl: null, onComplete: function(swaggerApi, swaggerUi){}, onFailure: function(data){log(\"Unable to Load SwaggerUI\");}, docExpansion: \"none\", jsonEditor: false, defaultModelRendering: 'schema', showRequestHeaders: false, showOperationIds: false}); window.swaggerUi.load(); function log(){if ('console' in window){console.log.apply(console, arguments);}}}); </script></head><body class=\"swagger-section\"><div id='header'> <div class=\"swagger-ui-wrap\"> <a id=\"logo\" href=\"http://swagger.io\"><img class=\"logo__img\" alt=\"swagger\" height=\"30\" width=\"30\" src=\"https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/images/logo_small.png\"/><span class=\"logo__title\">swagger</span></a> <form id='api_selector'> </form> </div></div><div id=\"message-bar\" class=\"swagger-ui-wrap\" data-sw-translate>&nbsp;</div><div id=\"swagger-ui-container\" class=\"swagger-ui-wrap\"></div></body></html>/cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/css/screen.css' media='screen' rel='stylesheet' type='text/css'/> <script>if (typeof Object.assign !='function'){(function (){Object.assign=function (target){'use strict'; if (target===undefined || target===null){throw new TypeError('Cannot convert undefined or null to object');}var output=Object(target); for (var index=1; index < arguments.length; index++){var source=arguments[index]; if (source !==undefined && source !==null){for (var nextKey in source){if (Object.prototype.hasOwnProperty.call(source, nextKey)){output[nextKey]=source[nextKey];}}}}return output;};})();}</script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/1.8.0/jquery-1.8.0.min.js' type='text/javascript'></script> <script>(function(b){b.fn.slideto=function(a){a=b.extend({slide_duration:\"slow\",highlight_duration:3E3,highlight:true,highlight_color:\"#FFFF99\"},a);return this.each(function(){obj=b(this);b(\"body\").animate({scrollTop:obj.offset().top},a.slide_duration,function(){a.highlight&&b.ui.version&&obj.effect(\"highlight\",{color:a.highlight_color},a.highlight_duration)})})}})(jQuery); </script> <script>jQuery.fn.wiggle=function(o){var d={speed:50,wiggles:3,travel:5,callback:null};var o=jQuery.extend(d,o);return this.each(function(){var cache=this;var wrap=jQuery(this).wrap('<div class=\"wiggle-wrap\"></div>').css(\"position\",\"relative\");var calls=0;for(i=1;i<=o.wiggles;i++){jQuery(this).animate({left:\"-=\"+o.travel},o.speed).animate({left:\"+=\"+o.travel*2},o.speed*2).animate({left:\"-=\"+o.travel},o.speed,function(){calls++;if(jQuery(cache).parent().hasClass('wiggle-wrap')){jQuery(cache).parent().replaceWith(cache);}if(calls==o.wiggles&&jQuery.isFunction(o.callback)){o.callback();}});}});}; </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery.ba-bbq/1.2.1/jquery.ba-bbq.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/handlebars.js/4.0.5/handlebars.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/lodash-compat/3.10.1/lodash.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/backbone.js/1.1.2/backbone-min.js' type='text/javascript'></script> <script>Backbone.View=(function(View){return View.extend({constructor: function(options){this.options=options ||{}; View.apply(this, arguments);}});})(Backbone.View); </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/swagger-ui.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/highlight.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/languages/json.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/json-editor/0.7.28/jsoneditor.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/marked/0.3.6/marked.min.js' type='text/javascript'></script> <script type=\"text/javascript\">$(function (){url=\"http://192.168.1.40/swagger.json\"; hljs.configure({highlightSizeThreshold: 5000}); window.swaggerUi=new SwaggerUi({url: url, dom_id: \"swagger-ui-container\", supportedSubmitMethods: ['get', 'post', 'put', 'delete', 'patch'],validatorUrl: null, onComplete: function(swaggerApi, swaggerUi){}, onFailure: function(data){log(\"Unable to Load SwaggerUI\");}, docExpansion: \"none\", jsonEditor: false, defaultModelRendering: 'schema', showRequestHeaders: false, showOperationIds: false}); window.swaggerUi.load(); function log(){if ('console' in window){console.log.apply(console, arguments);}}}); </script></head><body class=\"swagger-section\"><div id='header'> <div class=\"swagger-ui-wrap\"> <a id=\"logo\" href=\"http://swagger.io\"><img class=\"logo__img\" alt=\"swagger\" height=\"30\" width=\"30\" src=\"https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/images/logo_small.png\"/><span class=\"logo__title\">swagger</span></a> <form id='api_selector_section'> </form> </div></div><div id=\"message-bar\" class=\"swagger-ui-wrap\" data-sw-translate>&nbsp;</div><div id=\"swagger-ui-container\" class=\"swagger-ui-wrap\"></div></body></html>";


void handleRootPath() {            //Handler for the rooth path
  server.send(200, "text/plain", "Hello world");
}

void handleSheepState() { 
  String response = state.respondWithSheepState();
  server.send(200, "application/json", response); 
}
 
void handleSwaggerJson() { 
  server.send(200, "application/json", swaggerJSON);
}
 
void handleSwaggerUI() { 
  server.send(200, "text/html", swaggerUI);
}



class WebServer
{
  private:
    
  public:  


    WebServer(){      
    }

    void init(){

      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  //Print the local IP
     
      server.on("/other", []() {   //Define the handling function for the path
     
        server.send(200, "text / plain", "Other URL");
     
      });
     
      server.on("/", handleRootPath);    //Associate the handler function to the path
      server.on("/sheep/state", handleSheepState);
      server.on("/swagger.json", handleSwaggerJson);
      server.on("/swaggerUI", handleSwaggerUI); 
           
      server.begin();                    //Start the server
      Serial.println("Server listening");
            
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


    void handleClient(){
      server.handleClient();      
    }

    
    /*
     * 
     */
    void listenForIncomingRequests(){
/*      
 *       WiFiClient client = server.available();
      if (client)
      {
        handleClientRequest(client);
    
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
      } 
       
      // The client will actually be disconnected 
      // when the function returns and 'client' object is detroyed
*/      
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
