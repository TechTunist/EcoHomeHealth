#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "SimpleEspNowConnection.h"
#include "DHT.h"
#include <ArduinoJson.h>

#define Type DHT11

AsyncWebServer server(80);

const char* ssid = "TNCAP24C3C5";
const char* password = "7EFEF61DDA";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP-NOW DASHBOARD</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h1 {  font-size: 2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #2f4468; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .timestamp { color: #bebebe; font-size: 1rem; }
    .card-title{ font-size: 1.2rem; font-weight : bold; }
    .card.temperature { color: #B10F2E; }
    .card.humidity { color: #50B8B4; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>ESP-NOW DASHBOARD</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <p class="card-title"><i class="fas fa-thermometer-half"></i> BOARD #1 - TEMPERATURE</p><p><span class="reading"><span id="t1"></span> &deg;C</span></p><p class="timestamp">Last Reading: <span id="rt1"></span></p>
      </div>
      <div class="card humidity">
        <p class="card-title"><i class="fas fa-tint"></i> BOARD #1 - HUMIDITY</p><p><span class="reading"><span id="h1"></span> &percnt;</span></p><p class="timestamp">Last Reading: <span id="rh1"></span></p>
      </div>
      <div class="card temperature">
        <p class="card-title"><i class="fas fa-thermometer-half"></i> BOARD #2 - TEMPERATURE</p><p><span class="reading"><span id="t2"></span> &deg;C</span></p><p class="timestamp">Last Reading: <span id="rt2"></span></p>
      </div>
      <div class="card humidity">
        <p class="card-title"><i class="fas fa-tint"></i> BOARD #2 - HUMIDITY</p><p><span class="reading"><span id="h2"></span> &percnt;</span></p><p class="timestamp">Last Reading: <span id="rh2"></span></p>
      </div>
    </div>
  </div>
<script>
function getDateTime() {
  var currentdate = new Date();
  var datetime = currentdate.getDate() + "/"
  + (currentdate.getMonth()+1) + "/"
  + currentdate.getFullYear() + " at "
  + currentdate.getHours() + ":"
  + currentdate.getMinutes() + ":"
  + currentdate.getSeconds();
  return datetime;
}
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('new_readings', function(e) {
  console.log("new_readings", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("t"+obj.id).innerHTML = obj.temperature.toFixed(2);
  document.getElementById("h"+obj.id).innerHTML = obj.humidity.toFixed(2);
  document.getElementById("rt"+obj.id).innerHTML = getDateTime();
  document.getElementById("rh"+obj.id).innerHTML = getDateTime();
 }, false);
}
</script>
</body>
</html>)rawliteral";


void setup() {

    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", index_html);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
}





//// utility function to serve the home page dashboard
//void get_index() {
//  String html = "<!DOCTYPE html> <html> ";
//  html += "<head><meta http-equiv=\"refresh\" content=\"2\"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
//  html += "<body> <h1>The EcoHomeHealth Dashboard</h1>";
//  html += "<p> Welcome to the Future of Efficiency</p>";
//  html += "<div> <p> <strong> The Temperature of internal sensor 1 is: ";
//  // html += HT.readTemperature();
////  html += temp;
//  html += " C</strong> </p> </div>";
//  html += "<div> <p> <strong> The Humidity of internal sensor 1 is: ";
//  // html += HT.readHumidity();
////  html += humidity;
//  html += " %</strong> </p> </div>";
//  //// enter ternery operator here to say if the temperature is above or below threshold;
//  // isAboveThreshold()?"is currently above threshold":"is currently below threshold";
//  html += "<a href=\"/setLEDStatus?s=0\" target=\"_blank\"\"\"><button>Turn Off </button></a>";
//  html += "<a href=\"/setLEDStatus?s=1\" target=\"_blank\"\"\"><button>Turn On </button></a>";
//
//  html += "<div> <p> <strong> The Temperature of satellite sensor 1 is: ";
//  // html += HT.readTemperature();
//  
//  html += " C</strong> </p> </div>";
//  
//  html += "</body> </html>";
//
//  // print a welcoming message on the index page
//  server.send(200, "text.html", html);
//}
