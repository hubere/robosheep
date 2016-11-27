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
<!DOCTYPE HTML>\ 
<html>\ 
<head>\ 
<style>\ 
div {\ 
    width: 100px;\ 
    height: 200px;\ 
    border: 1px solid black;\ 
}\ 
</style>\ 
</head>\ 
<body>\ 
\ 
<h1>Motordriver</h1>\ 
\ 
<p><strong>Tip:</strong> Try to click different places in the box to speed up motors.<br> \ 
Center means 0,0 i.e. stop. Click towards top runs forward, a click towards bottom runs backward.\ 
Once the mouse moves out of the box, motors come to a still.\ 
</p>\ 
\ 
Enable Motor: <input type=\"checkbox\" id=\"enableMotor\">\ 
\ 
\ 
<div onclick=\"setMousePosition(event)\" onmousemove=\"speedMotors(event)\" onmouseout=\"stop()\"></div>\ 
\ 
<p id=\"demo\"></p>\ 
\ 
<script>\ 
\ 
var offsetX ;\ 
var offsetY ;\ 
var x;\ 
var y;\ 
\ 
function setMousePosition(event) {\ 
	console.log(\"setMousePosition called \");\ 
    x = event.clientX;\ 
    y = event.clientY;\ 
    offsetX = x ;\ 
    offsetY = y ;\ 
    var response = \"Offest (\"+x+\"/\"+y+\")\";\ 
    document.getElementById('demo').innerHTML = response;\ 
}\ 
\ 
function speedMotors(event) {\ 
    x = event.clientX;\ 
    y = event.clientY;\ 
	var m1 = 0;\ 
	var m2 = 0;\ 
	if (offsetX != 0)\ 
	{\ 
		var divX = offsetX - x;\ 
		var divY = offsetY - y;\ 
		m1 = divY  + divX;\ 
		m2 = divY  - divX;\ 
	}\ 
	send(m1,m2);\ 
}\ 
\ 
function stop() {\ 
    offsetX = 0;\ 
    offsetY = 0;\ 
	x = 0;\ 
	y = 0;\ 
	send(0,0);\ 
}\ 
\ 
function send(m1, m2) {\ 
  var motorsEnabled = document.getElementById(\"enableMotor\").checked;\ 
  var response = \"Offest (\"+offsetX+\"/\"+offsetY+\") m1:\"+m1+\" m2:\"+m2;\ 
  if (motorsEnabled)\ 
  {\ 
	  var xmlHttp = new XMLHttpRequest();\ 
	  xmlHttp.open( \"GET\", \"http://192.168.0.112/motor?m1=\"+m1+\"&m2=\"+m2, false ); \ 
	  xmlHttp.send( null );\ 
	  response = xmlHttp.responseText;\ 
  }\ 
  document.getElementById('demo').innerHTML = response;\ 
}\ 
\ 
</script>\ 
</body>\ 
</html>\ 
";


#endif
