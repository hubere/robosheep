#ifndef WEB_SERVER_h
#define WEB_SERVER_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


#define RESP_STATUS_REQUESTED      0
#define RESP_COMMAND_WAS_ISSUED    1

ESP8266WebServer server(80);

const char * swaggerJSON = "{\"swagger\":\"2.0\",\"info\":{\"description\":\"This is the REST interface to the sheep.\",\"version\":\"1.0.0\",\"title\":\"Robosheep\"},\"host\":\"192.168.1.40\",\"tags\":[{\"name\":\"State\",\"description\":\"Getting state of sheep\"},{\"name\":\"Move\",\"description\":\"Make sheep move\"},{\"name\":\"Cutter\",\"description\":\"Make sheep eat\"}],\"paths\":{\"/sheep/state\":{\"get\":{\"tags\":[\"State\"],\"summary\":\"Endpoint for getting internal state of sheep\",\"description\":\"\",\"operationId\":\"getSheep\",\"responses\":{\"200\":{\"description\":\"Actual state of sheep\",\"schema\":{\"$ref\":\"#/definitions/sheepState\"}}}}},\"/sheep/set\":{\"get\":{\"tags\":[\"State\"],\"summary\":\"Endpoint for setting certain parameters of sheep\",\"description\":\"\",\"operationId\":\"setParameter\",\"parameters\":[{\"in\":\"query\",\"name\":\"maxSpeed\",\"type\":\"integer\",\"description\":\"The number of items to skip before starting to collect the result set\"},{\"in\":\"query\",\"name\":\"errorM1\",\"type\":\"integer\",\"description\":\"Error of motor M1. This value is added to output of current for M1\"},{\"in\":\"query\",\"name\":\"errorM2\",\"type\":\"integer\",\"description\":\"Error of motor M2. This value is added to output of current for M2\"}],\"responses\":{\"200\":{\"description\":\"Set paramerter of sheep\"}}}},\"/sheep/move\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep by setting speed and direction.\",\"description\":\"Description of Endpoint for moving sheep\",\"parameters\":[{\"in\":\"query\",\"name\":\"speed\",\"type\":\"integer\",\"description\":\"Speed to set for both motors in percentage.\"},{\"in\":\"query\",\"name\":\"dir\",\"type\":\"integer\",\"description\":\"Amount of difference of motors speed in percentage. dir=10 will increase M1 by 10% and decrease M2 by 10%\"}],\"responses\":{\"200\":{\"description\":\"Sheep is moving\"}}}},\"/sheep/motor\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep by setting speed and direction.\",\"description\":\"Description of Endpoint for moving sheep\",\"parameters\":[{\"in\":\"query\",\"name\":\"m1\",\"type\":\"integer\",\"description\":\"Speed to set to motor M1 in percentage.\"},{\"in\":\"query\",\"name\":\"m2\",\"type\":\"integer\",\"description\":\"Speed to set to motor M2 in percentage.\"}],\"responses\":{\"200\":{\"description\":\"Sheep is moving\"}}}},\"/sheep/forward\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep forward\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving forward\"}}}},\"/sheep/backward\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep backward\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving backward\"}}}},\"/sheep/left\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep left\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving left\"}}}},\"/sheep/right\":{\"get\":{\"tags\":[\"Move\"],\"summary\":\"Endpoint for moving sheep right\",\"description\":\"Description of Endpoint for moving sheep\",\"responses\":{\"200\":{\"description\":\"Sheep is moving right\"}}}},\"/cutter/on\":{\"get\":{\"tags\":[\"Cutter\"],\"summary\":\"Endpoint starting cutter\",\"description\":\"\",\"responses\":{\"200\":{\"description\":\"Cutter is running\"}}}},\"/cutter/off\":{\"get\":{\"tags\":[\"Cutter\"],\"summary\":\"Endpoint for stopping cutter\",\"description\":\"\",\"responses\":{\"200\":{\"description\":\"Cutter is shut down\"}}}}},\"definitions\":{\"sheepState\":{\"type\":\"object\",\"properties\":{\"m1\":{\"type\":\"integer\"},\"m2\":{\"type\":\"integer\"},\"desiredSpeedM1\":{\"type\":\"integer\"},\"desiredSpeedM2\":{\"type\":\"integer\"},\"posM1\":{\"type\":\"integer\"},\"posM2\":{\"type\":\"integer\"},\"errorM1\":{\"type\":\"integer\"},\"errorM2\":{\"type\":\"integer\"},\"maxSpeed\":{\"type\":\"integer\"},\"isCutterOn\":{\"type\":\"boolean\"},\"power\":{\"type\":\"integer\"},\"rssi\":{\"type\":\"integer\"},\"losingConnection\":{\"type\":\"integer\"}}}}}";
const char * swaggerUI = " <!DOCTYPE html><html><head> <meta charset=\"UTF-8\"> <meta http-equiv=\"x-ua-compatible\" content=\"IE=edge\"> <title>Swagger UI</title> <link href='https://cdnjs.cloudflare.com/ajax/libs/meyer-reset/2.0/reset.min.css' media='screen' rel='stylesheet' type='text/css'/> <link href='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/css/screen.css' media='screen' rel='stylesheet' type='text/css'/> <script>if (typeof Object.assign !='function'){(function (){Object.assign=function (target){'use strict'; if (target===undefined || target===null){throw new TypeError('Cannot convert undefined or null to object');}var output=Object(target); for (var index=1; index < arguments.length; index++){var source=arguments[index]; if (source !==undefined && source !==null){for (var nextKey in source){if (Object.prototype.hasOwnProperty.call(source, nextKey)){output[nextKey]=source[nextKey];}}}}return output;};})();}</script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/1.8.0/jquery-1.8.0.min.js' type='text/javascript'></script> <script>(function(b){b.fn.slideto=function(a){a=b.extend({slide_duration:\"slow\",highlight_duration:3E3,highlight:true,highlight_color:\"#FFFF99\"},a);return this.each(function(){obj=b(this);b(\"body\").animate({scrollTop:obj.offset().top},a.slide_duration,function(){a.highlight&&b.ui.version&&obj.effect(\"highlight\",{color:a.highlight_color},a.highlight_duration)})})}})(jQuery); </script> <script>jQuery.fn.wiggle=function(o){var d={speed:50,wiggles:3,travel:5,callback:null};var o=jQuery.extend(d,o);return this.each(function(){var cache=this;var wrap=jQuery(this).wrap('<div class=\"wiggle-wrap\"></div>').css(\"position\",\"relative\");var calls=0;for(i=1;i<=o.wiggles;i++){jQuery(this).animate({left:\"-=\"+o.travel},o.speed).animate({left:\"+=\"+o.travel*2},o.speed*2).animate({left:\"-=\"+o.travel},o.speed,function(){calls++;if(jQuery(cache).parent().hasClass('wiggle-wrap')){jQuery(cache).parent().replaceWith(cache);}if(calls==o.wiggles&&jQuery.isFunction(o.callback)){o.callback();}});}});}; </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery.ba-bbq/1.2.1/jquery.ba-bbq.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/handlebars.js/4.0.5/handlebars.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/lodash-compat/3.10.1/lodash.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/backbone.js/1.1.2/backbone-min.js' type='text/javascript'></script> <script>Backbone.View=(function(View){return View.extend({constructor: function(options){this.options=options ||{}; View.apply(this, arguments);}});})(Backbone.View); </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/swagger-ui.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/highlight.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/languages/json.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/json-editor/0.7.28/jsoneditor.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/marked/0.3.6/marked.min.js' type='text/javascript'></script> <script type=\"text/javascript\">$(function (){url=\"http://192.168.1.40/swagger.json\"; hljs.configure({highlightSizeThreshold: 5000}); window.swaggerUi=new SwaggerUi({url: url, dom_id: \"swagger-ui-container\", supportedSubmitMethods: ['get', 'post', 'put', 'delete', 'patch'],validatorUrl: null, onComplete: function(swaggerApi, swaggerUi){}, onFailure: function(data){log(\"Unable to Load SwaggerUI\");}, docExpansion: \"none\", jsonEditor: false, defaultModelRendering: 'schema', showRequestHeaders: false, showOperationIds: false}); window.swaggerUi.load(); function log(){if ('console' in window){console.log.apply(console, arguments);}}}); </script></head><body class=\"swagger-section\"><div id='header'> <div class=\"swagger-ui-wrap\"> <a id=\"logo\" href=\"http://swagger.io\"><img class=\"logo__img\" alt=\"swagger\" height=\"30\" width=\"30\" src=\"https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/images/logo_small.png\"/><span class=\"logo__title\">swagger</span></a> <form id='api_selector'> </form> </div></div><div id=\"message-bar\" class=\"swagger-ui-wrap\" data-sw-translate>&nbsp;</div><div id=\"swagger-ui-container\" class=\"swagger-ui-wrap\"></div></body></html>/cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/css/screen.css' media='screen' rel='stylesheet' type='text/css'/> <script>if (typeof Object.assign !='function'){(function (){Object.assign=function (target){'use strict'; if (target===undefined || target===null){throw new TypeError('Cannot convert undefined or null to object');}var output=Object(target); for (var index=1; index < arguments.length; index++){var source=arguments[index]; if (source !==undefined && source !==null){for (var nextKey in source){if (Object.prototype.hasOwnProperty.call(source, nextKey)){output[nextKey]=source[nextKey];}}}}return output;};})();}</script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/1.8.0/jquery-1.8.0.min.js' type='text/javascript'></script> <script>(function(b){b.fn.slideto=function(a){a=b.extend({slide_duration:\"slow\",highlight_duration:3E3,highlight:true,highlight_color:\"#FFFF99\"},a);return this.each(function(){obj=b(this);b(\"body\").animate({scrollTop:obj.offset().top},a.slide_duration,function(){a.highlight&&b.ui.version&&obj.effect(\"highlight\",{color:a.highlight_color},a.highlight_duration)})})}})(jQuery); </script> <script>jQuery.fn.wiggle=function(o){var d={speed:50,wiggles:3,travel:5,callback:null};var o=jQuery.extend(d,o);return this.each(function(){var cache=this;var wrap=jQuery(this).wrap('<div class=\"wiggle-wrap\"></div>').css(\"position\",\"relative\");var calls=0;for(i=1;i<=o.wiggles;i++){jQuery(this).animate({left:\"-=\"+o.travel},o.speed).animate({left:\"+=\"+o.travel*2},o.speed*2).animate({left:\"-=\"+o.travel},o.speed,function(){calls++;if(jQuery(cache).parent().hasClass('wiggle-wrap')){jQuery(cache).parent().replaceWith(cache);}if(calls==o.wiggles&&jQuery.isFunction(o.callback)){o.callback();}});}});}; </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery.ba-bbq/1.2.1/jquery.ba-bbq.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/handlebars.js/4.0.5/handlebars.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/lodash-compat/3.10.1/lodash.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/backbone.js/1.1.2/backbone-min.js' type='text/javascript'></script> <script>Backbone.View=(function(View){return View.extend({constructor: function(options){this.options=options ||{}; View.apply(this, arguments);}});})(Backbone.View); </script> <script src='https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/swagger-ui.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/highlight.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.10.0/languages/json.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/json-editor/0.7.28/jsoneditor.min.js' type='text/javascript'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/marked/0.3.6/marked.min.js' type='text/javascript'></script> <script type=\"text/javascript\">$(function (){url=\"http://192.168.1.40/swagger.json\"; hljs.configure({highlightSizeThreshold: 5000}); window.swaggerUi=new SwaggerUi({url: url, dom_id: \"swagger-ui-container\", supportedSubmitMethods: ['get', 'post', 'put', 'delete', 'patch'],validatorUrl: null, onComplete: function(swaggerApi, swaggerUi){}, onFailure: function(data){log(\"Unable to Load SwaggerUI\");}, docExpansion: \"none\", jsonEditor: false, defaultModelRendering: 'schema', showRequestHeaders: false, showOperationIds: false}); window.swaggerUi.load(); function log(){if ('console' in window){console.log.apply(console, arguments);}}}); </script></head><body class=\"swagger-section\"><div id='header'> <div class=\"swagger-ui-wrap\"> <a id=\"logo\" href=\"http://swagger.io\"><img class=\"logo__img\" alt=\"swagger\" height=\"30\" width=\"30\" src=\"https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/2.2.10/images/logo_small.png\"/><span class=\"logo__title\">swagger</span></a> <form id='api_selector_section'> </form> </div></div><div id=\"message-bar\" class=\"swagger-ui-wrap\" data-sw-translate>&nbsp;</div><div id=\"swagger-ui-container\" class=\"swagger-ui-wrap\"></div></body></html>";
const char * page_index =        "<!-- To use this html in Arduino, 0) 1) replace 192.168.1.40 with local IP of web server 2a) remove all line comments in <script>2b) ensure all lines are terminated by a semicolon 2c) use https://www.willpeavy.com/tools/minifier/ to have this html as one line 3) replace '\"' with '\"' in this one line string.--><!DOCTYPE HTML><html><head> <title>Robosheep</title> <meta charset=\"utf-8\"/> <style>#motor_speed_bars{width: 600px; padding: 10px; background-color: grey;}button{height: 80px; width: 200px;}#speedBar{color: green;}.progress-bar-vertical{width: 30px; min-height: 100px; display: flex; align-items: flex-end; margin-right: 20px; float: left;}.progress-bar-vertical .progress-bar{width: 100%; height: 0; color: black; -webkit-transition: height 0.6s ease; -o-transition: height 0.6s ease; transition: height 0.6s ease;}</style></head><body><link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" rel=\"stylesheet\"/><h1>Motordriver (Ver. 0.9)</h1><div style=\"margin-right: auto; margin-left: auto; width: 602px; border: 1px solid black;\"> <h2>Robosheep Status</h2> <div id=\"motor_speed_bars\"> <div>M1___M2__RSSI__PWR </div><div class=\"progress progress-bar-vertical\"> <div id=\"motorBar1\" class=\"progress-bar\" role=\"progressbar\" style=\"height: 50%;\" aria-valuenow=\"50\" aria-valuemin=\"0\" aria-valuemax=\"100\">0% </div></div><div class=\"progress progress-bar-vertical\"> <div id=\"motorBar2\" class=\"progress-bar\" role=\"progressbar\" style=\"height: 50%;\" aria-valuenow=\"50\" aria-valuemin=\"0\" aria-valuemax=\"100\">0% </div></div><div class=\"progress progress-bar-vertical\"> <div id=\"signalStrength\" class=\"progress-bar\" role=\"progressbar\" style=\"height: 0%;\" aria-valuenow=\"0\" aria-valuemin=\"0\" aria-valuemax=\"100\">0% </div></div><div class=\"progress progress-bar-vertical\"> <div id=\"powerSupply\" class=\"progress-bar\" role=\"progressbar\" style=\"height: 0%;\" aria-valuenow=\"0\" aria-valuemin=\"0\" aria-valuemax=\"100\">0% </div></div><br><br><br><br><br><br></div><p id=\"sheep_state\">sheep_state</p></div><div style=\"width: 602px; margin: auto; border: 1px solid black;\"> <h2>Joystick</h2> <div id=\"joystick\" style=\"position:relative; border: 1px solid black; width: 600px; height: 600px; -webkit-border-radius: 300px; -moz-border-radius: 300px; border-radius: 300px; background: green;\"> <img id=\"ball\" src=\"https://en.js.cx/clipart/ball.svg\" style=\"position:absolute; top:260px; left:260px; width:80px; height:80px;\"/> </div></div><div style=\"width: 608px; margin: auto; border: 1px solid black;\"> <h2>Control</h2> <TABLE> <TR> <TD> <p><label for=\"enableMotor\">Enable Motor: </label><input style=\"height:20px;width:20px\" type=\"checkbox\" id=\"enableMotor\"></p></TD> <TD> <button onclick=\"forward()\">forward</button> </TD> <TD></TD> </TR> <TR> <TD> <button onclick=\"left()\">left</button> </TD> <TD> <button onclick=\"stop()\">stop</button> </TD> <TD> <button onclick=\"right()\">right</button> </TD> </TR> <TR> <TD></TD> <TD> <button onclick=\"backward()\">backward</button> </TD> <TD></TD> </TR> <TR> <TD> <button onclick=\"speed1()\">speed1</button> </TD> <TD> <button onclick=\"speed2()\">speed2</button> </TD> <TD> <button onclick=\"speed3()\">speed3</button> </TD> </TR> </TABLE></div><!--<div style=\"display: table-cell; border: 1px solid black;\">--><div style=\"width: 600px; margin: auto; border: 1px solid black;\"> <h2>Log</h2> <p id=\"log\">log</p></div><script>/* * global vars */ let ip=location.host; if (location.host.startsWith(\"localhost\")) ip=\"192.168.1.40\"; console.log(\"communication with ip: \" + ip); const url_sheep_state=\"http://\" + ip + \"/sheep/state\"; const url_sheep_move=\"http://\" + ip + \"/sheep/move\"; const url_motor=\"http://\" + ip + \"/motor\"; var speed=0; console.log(\"speed set to \" + speed); /* * motor bars */ const motor_speed_bars=document.getElementById(\"motor_speed_bars\"); const speedBar=document.getElementById('speedBar'); const motorBar1=document.getElementById('motorBar1'); const motorBar2=document.getElementById('motorBar2'); const sheepStateCtrl=document.getElementById('sheep_state'); const logger=document.getElementById('log'); const startX=ball.getBoundingClientRect().left; const startY=ball.getBoundingClientRect().top; motor_speed_bars.onmousedown=function (event){console.log(\"onmousedown: \" + event.button);}; /* Update progress bars that indicate motor speed. Values are between -100 and 100. */ function updateMotorBars(m1, m2){if (m1 < -100 || m1 > 100 || m2 < -100 || m2 > 100){console.error(\"Called updateMotorBars(\" + m1 + \",\" + m2 + \") with invalid values.\"); return;}updateMotorBar(motorBar1, m1); updateMotorBar(motorBar2, m2);}; function updateMotorBar(motorBar, value){width=(100 + value)/2; motorBar.style.height=width+'%'; motorBar.innerHTML=value+'%'; if (value < 0){motorBar.style.backgroundColor=\"red\";}else{motorBar.style.backgroundColor=\"green\";}}; function updateRssiCtrl(sheep_state){let rssiValue=parseInt(sheep_state['rssi']); let barHeight=100 + rssiValue; const signalStrengthCtrl=document.getElementById('signalStrength'); signalStrengthCtrl.style.height=barHeight+'%'; signalStrengthCtrl.innerHTML=rssiValue + 'dBm';}; function updatePwrCtrl(sheep_state){let powerValue=parseInt(sheep_state['power']); let barHeight=powerValue / 24 * 100; if (barHeight > 100) barHeight=100; let voltage=(powerValue / 10.0).toFixed(1); const powerSupplyCtrl=document.getElementById('powerSupply'); powerSupplyCtrl.style.height=barHeight+'%'; powerSupplyCtrl.innerHTML=voltage + 'V'; /* console.log(\"Voltage: \" + voltage) */}; /* * update sheep state */ const intervalID=setInterval(function (){const Http=new XMLHttpRequest(); Http.open(\"GET\", url_sheep_state); Http.responseType='json'; Http.send(); Http.onreadystatechange=function (){if (this.readyState===4 && this.status===200){let sheep_state=Http.response; try{updateMotorBars(parseInt(sheep_state['m1']), parseInt(sheep_state['m2']));}catch(e){console.error(\"Error parseInt(sheep_state[m1]): sheep_state=\" + JSON.stringify(sheep_state));}updateRssiCtrl(sheep_state); updatePwrCtrl(sheep_state); sheepStateCtrl.innerHTML=JSON.stringify(sheep_state);}}}, 1000); /* * send command to sheep */ let awaitingRequest=false; function sendCommand(speed, direction){/* ensure, there is a delay of at least 1000ms between consecutive commands */ if (awaitingRequest) return; awaitingRequest=true; setTimeout(function (){awaitingRequest=false;}, 1000); let sum=direction + speed; let diff=direction - speed; if (Math.abs(sum) > 100 || Math.abs(diff) > 100){console.log(\"motor speeds to high\"); return;}/* call server */ let url=url_sheep_move + \"?speed=\" + speed + \"&dir=\" + direction; console.log(\"Sending: \" + url ); const Http=new XMLHttpRequest(); Http.open(\"GET\", url); Http.responseType='json'; Http.send();}; /* * ball */ ball.onmousedown=function (event){let shiftX=event.clientX - ball.getBoundingClientRect().left; let shiftY=event.clientY - ball.getBoundingClientRect().top; ball.style.position='absolute'; ball.style.zIndex='1000'; document.body.append(ball); moveAt(event.pageX, event.pageY); /* * moves the ball at (pageX, pageY) coordinates * taking initial shifts into account */ function moveAt(pageX, pageY){/* * move ball image */ ball.style.left=pageX - shiftX + 'px'; ball.style.top=pageY - shiftY + 'px'; /* calc displacement */ let displaceX=-Math.trunc((pageX - startX - shiftX) / 3); let displaceY=-Math.trunc((pageY - startY - shiftY) / 3); var msg=\"Touch displace \" + displaceX + \"/\" + displaceY; console.log(msg); logger.innerHTML=msg; let speed=displaceY; let dir=-displaceX; sendCommand(speed, dir);}; function onMouseMove(event){moveAt(event.pageX, event.pageY);}; /* move the ball on mousemove */ document.addEventListener('mousemove', onMouseMove); /* drop the ball, remove unneeded handlers */ ball.onmouseup=function (){document.removeEventListener('mousemove', onMouseMove); ball.onmouseup=null; ball.style.left=startX + 'px'; ball.style.top=startY + 'px'; awaitingRequest=false; sendCommand(0,0);};}; /* * see also (https://www.mediaevent.de/javascript/touch-events.html) */ ball.ontouchmove=function (e){/* grab the location of touch */ var touchLocation=e.targetTouches[0]; /* erster finger */ /* * assign box new coordinates based on the touch. * ball.style.left=touchLocation.pageX + 'px'; * ball.style.top=touchLocation.pageY + 'px'; */ /* calc displacement */ let displaceX=Math.trunc((startX - touchLocation.pageX) / 3); let displaceY=Math.trunc((startY - touchLocation.pageY) / 3); console.log(\"Touch displace \" + displaceX + \"/\" + displaceY); var msg=\"Touch displace \" + displaceX + \"/\" + displaceY; logger.innerHTML=msg; let speed=displaceY; let dir=-displaceX; sendCommand(speed, dir);}; ball.ontouchend=function (e){sendCommand(0, 0);}; /* * Disable context menu */ window.oncontextmenu=function (event){event.preventDefault(); event.stopPropagation(); return false;}; ball.ondragstart=function (){return false;}; function speed1(){speed=12; console.log(\"speed set to \" + speed);}; function speed2(){speed=25; console.log(\"speed set to \" + speed);}; function speed3(){speed=100; console.log(\"speed set to \" + speed);}; function stop(){sendCommand(0, 0);}; function forward(){sendCommand(speed, 0);}; function left(){sendCommand(0, -speed);}; function right(){sendCommand(0, speed);}; function backward(){sendCommand(-speed, 0);}; var offsetX; var offsetY; var x; var y; function stop(){offsetX=0; offsetY=0; x=0; y=0; sendCommand(0, 0);};</script></body></html>";
const char * page_sheep_status = "";
const char * page_joystick =     "";
const char * page_control =      "";
const char * page_log =          "";


void handleSheepState() { 
  server.send(200, "application/json", state.respondWithSheepState()); 
}
 
void handleSheepSet() { 
  server.send(200, "text/html", ""); 
}
 
void handleSheepMove() { 
  server.send(200, "text/html", ""); 
}
 
void handleSheepMotor() { 
  server.send(200, "text/html", ""); 
}
 
void handleCutterOn() { 
  server.send(200, "text/html", "Cutter on"); 
}
 
void handleCutterOff() { 
  server.send(200, "text/html", "Cutter off"); 
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
          
      server.on("/"        , []() { server.send(200, "text/html", page_index);       });
      server.on("/status"  , []() { server.send(200, "text/html", page_sheep_status);});
      server.on("/joystick", []() { server.send(200, "text/html", page_joystick);    });
      server.on("/control" , []() { server.send(200, "text/html", page_control);     });
      server.on("/log"     , []() { server.send(200, "text/html", page_log);         });
      
      server.on("/sheep/state", handleSheepState);
      server.on("/sheep/set", handleSheepSet);
      server.on("/sheep/move", handleSheepMove);
      server.on("/sheep/motor", handleSheepMotor);
      server.on("/sheep/cutter/on", handleCutterOn);
      server.on("/sheep/cutter/off", handleCutterOff);
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
/*    int handleClientRequest(WiFiClient client)
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
              extractSetParameter();
              response = state.respondWithSheepState();          
              
            }else if (request.indexOf("/sheep/move") > 0){
              extractSpeedAndDir();
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
*/

    /*
     * A command was issued. Reset alive timer.
     */
    void commandIssued(){
      state.lastCommandTimestamp = millis(); // A command was issued, reset alive check timer
    }


    /*
     * Match the request, i.e. extract parameter to set and value
     */
    void extractSetParameter(){      
      String maxSpeedString = server.arg("maxSpeed");
      if (maxSpeedString != "")
      {
        state.maxSpeed =  maxSpeedString.toInt();
        Serial.println("Setting maxSpeed to "+ maxSpeedString);               
      }
      
      String errorM1String = server.arg("errorM1");
      if (errorM1String != "")
      {
        state.setErrorM1(errorM1String.toInt());
        Serial.println("Setting errorM1 to "+ errorM1String);               
      }
      
      String errorM2String = server.arg("errorM2");
      if (errorM2String != "")
      {
        state.setErrorM2(errorM2String.toInt());
        Serial.println("Setting errorM2 to "+ errorM2String);               
      }      
    }
    
    /*
     * Match the request, i.e. extract speed and dir
     * and set desiredSpeedM1 and desiredSpeedM2
     */
    void extractSpeedAndDir(String request){
      String speedString = server.arg("speed");
      String dirString = server.arg("dir");
      
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
      String distString = server.arg("dist");
      if (distString == NULL) return 0;
      return distString.toInt();      
    }
    
    /*
     * Match the request, i.e. extract speed for both motors,e.g. motor?m1=-10&m2=20
     */
    void extractMotorSpeeds(String request){
      int newDesireM1 = 0;
      int newDesireM2 = 0;
      String speedM1 = server.arg("m1");
      String speedM2 = server.arg("m2");
      if (speedM1 != NULL) newDesireM1 =  speedM1.toInt();
      if (speedM2 != NULL) newDesireM1 =  speedM2.toInt();
      
      state.setDesiredSpeeds(newDesireM1,newDesireM2);      
      commandIssued();                    
    }

    
};



WebServer webServer;


#endif
