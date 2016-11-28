#ifndef HTML_h
#define HTML_h

/**
 * 1) Write html-code
 * 2) remove all inline comments starting with '//'
 * 3) Replace '\r' with '\ \r'
 * 4) Replace '"' with '\"'
 * 5) Copy & paste the code into '<here comes the html>'
 */


const char *page1 = "\
<!DOCTYPE HTML>\n \ 
<html>\n \ 
<head>\n \ 
<meta charset=\"utf-8\"/>\n \ 
<style>\n \ 
div {\n \ 
    width: 100px;\n \ 
    height: 200px;\n \ 
    border: 1px solid black;\n \ 
}\n \ 
</style>\n \ 
</head>\n \ 
<body>\n \ 
\n \ 
<h1>Motordriver</h1>\n \ 
\n \ 
<p><strong>Tip:</strong> Try to click different places in the box to speed up motors.<br> \n \ 
Center means 0,0 i.e. stop. Click towards top runs forward, a click towards bottom runs backward.\n \ 
Once the mouse moves out of the box, motors come to a still.\n \ 
</p>\n \ 
\n \ 
Enable Motor: <input type=\"checkbox\" id=\"enableMotor\">\n \ 
\n \ 
\n \ 
<div onclick=\"setMousePosition(event)\" onmousemove=\"speedMotors(event)\" onmouseout=\"stop()\"></div>\n \ 
\n \ 
\n \ 
<TABLE BORDER=\"0\">\n \ 
<TR>\n \ 
<TD></TD>\n \ 
<TD><button onclick=\"forward()\">forward</button></TD>\n \ 
<TD></TD>\n \ 
</TR>\n \ 
<TR>\n \ 
<TD><button onclick=\"left()\">left</button></TD>\n \ 
<TD><button onclick=\"stop()\">stop</button></TD>\n \ 
<TD><button onclick=\"right()\">right</button></TD>\n \ 
</TR>\n \ 
<TR>\n \ 
<TD></TD>\n \ 
<TD><button onclick=\"backward()\">backward</button></TD>\n \ 
<TD></TD>\n \ 
</TR>\n \ 
<TR>\n \ 
<TD><button onclick=\"speed1()\">speed1</button></TD>\n \ 
<TD><button onclick=\"speed2()\">speed2</button></TD>\n \ 
<TD><button onclick=\"speed3()\">speed3</button></TD>\n \ 
</TR>\n \ 
\n \ 
</TABLE>\n \ 
\n \ 
\n \ 
\n \ 
<p id=\"demo\"></p>\n \ 
\n \ 
<script>\n \ 
\n \ 
var speed = 0;\n \ 
\n \ 
function speed1() {\n \ 
	speed = 25;\n \ 
}\n \ 
\n \ 
function speed2() {\n \ 
	speed = 50;\n \ 
}\n \ 
\n \ 
function speed3() {\n \ 
	speed = 100;\n \ 
}\n \ 
\n \ 
function stop() {\n \ 
	send(0, 0);\n \ 
}\n \ 
\n \ 
function forward() {\n \ 
	send(speed, speed);\n \ 
}\n \ 
\n \ 
function left() {\n \ 
	send(0, speed);\n \ 
}\n \ 
\n \ 
function right() {\n \ 
	send(speed, 0);\n \ 
}\n \ 
\n \ 
function backward() {\n \ 
	send(-speed, -speed);\n \ 
}\n \ 
\n \ 
\n \ 
\n \ 
\n \ 
\n \ 
\n \ 
var offsetX ;\n \ 
var offsetY ;\n \ 
var x;\n \ 
var y;\n \ 
\n \ 
function setMousePosition(event) {\n \ 
	console.log(\"setMousePosition called \");\n \ 
    x = event.clientX;\n \ 
    y = event.clientY;\n \ 
    offsetX = x ;\n \ 
    offsetY = y ;\n \ 
    var response = \"Offest (\"+x+\"/\"+y+\")\";\n \ 
    document.getElementById('demo').innerHTML = response;\n \ 
}\n \ 
\n \ 
function speedMotors(event) {\n \ 
    x = event.clientX;\n \ 
    y = event.clientY;\n \ 
	var m1 = 0;\n \ 
	var m2 = 0;\n \ 
	if (offsetX != 0)\n \ 
	{\n \ 
		var divX = offsetX - x;\n \ 
		var divY = offsetY - y;\n \ 
		m1 = divY  + divX;\n \ 
		m2 = divY  - divX;\n \ 
	}\n \ 
	send(m1,m2);\n \ 
}\n \ 
\n \ 
function stop() {\n \ 
    offsetX = 0;\n \ 
    offsetY = 0;\n \ 
	x = 0;\n \ 
	y = 0;\n \ 
	send(0,0);\n \ 
}\n \ 
\n \ 
function send(m1, m2) {\n \ 
  var motorsEnabled = document.getElementById(\"enableMotor\").checked;\n \ 
  var response = \"Offest (\"+offsetX+\"/\"+offsetY+\") m1:\"+m1+\" m2:\"+m2;\n \ 
  console.log(\"send() called\" + response);\n \ 
  if (motorsEnabled)\n \ 
  {\n \ 
	  var xmlHttp = new XMLHttpRequest();\n \ 
	  xmlHttp.open( \"GET\", \"http://192.168.0.112/motor?m1=\"+m1+\"&m2=\"+m2, false ); \n \ 
	  xmlHttp.send( null );\n \ 
	  response = xmlHttp.responseText;\n \ 
  }\n \ 
  document.getElementById('demo').innerHTML = response;\n \ 
}\n \ 
\n \ 
</script>\n \ 
</body>\n \ 
</html>\n \ 
";


#endif
