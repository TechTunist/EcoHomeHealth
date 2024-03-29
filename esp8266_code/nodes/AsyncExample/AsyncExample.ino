/*
This sketch implements an asynchronous web server that hosts a dashboard and
handles espnow connections to receive data packets from satellite nodes. 
 */

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino_JSON.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncTCP.h"
#include <ArduinoJson.h>
#include "AsyncJson.h"

DynamicJsonDocument board_one(1024);
DynamicJsonDocument board_two(1024);

// variable to keep track of which board sent the last readings
int board_id;

StaticJsonDocument<100> data;


// OFFICE
const char* ssid = "TNCAP24C3C5";
const char* password = "7EFEF61DDA";
//const char* ssid = "TP_9958";
//const char* password = "15295316";

// HOME
//const char* ssid = "SKYXIENC";
//const char* password = "7GQiqMQT6zdB";

// set led pin
const int led_board_one = 4; // D2
const int led_board_two = 5; // D1

// set temp to trigger LED
int pivot_temp = 23;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  float temp;
  float pressure;
  float altitude;
  float humidity;
  int readingId;
} struct_message;

struct_message satelliteData;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&satelliteData, incomingData, sizeof(satelliteData));

  board_id = satelliteData.id;

  if (satelliteData.id == 1){
    board_one["ID"] = board["id"];
    board_one["Temperature"] = satelliteData.temp;
    board_one["Pressure"] = satelliteData.pressure;
    board_one["Humidity"] = satelliteData.humidity;
    board_one["Altitude"] = satelliteData.altitude;
  }

  if (satelliteData.id == 2){
    board_two["ID"] = board["id"];
    board_two["Temperature"] = satelliteData.temp;
    board_two["Pressure"] = satelliteData.pressure;
    board_two["Humidity"] = satelliteData.humidity;
    board_two["Altitude"] = satelliteData.altitude;
  }
  
  board["id"] = satelliteData.id;
  board["temperature"] = satelliteData.temp;
  board["pressure"] = satelliteData.pressure;
  board["altitude"] = satelliteData.altitude;
  board["humidity"] = satelliteData.humidity;
  board["readingId"] = String(satelliteData.readingId);
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_data", millis());

  // set condition for lighting LED for both boards independently
  if (satelliteData.id == 1){
    if (satelliteData.temp > pivot_temp){
      digitalWrite(led_board_one, HIGH);
  } else {
      digitalWrite(led_board_one, LOW);
    }  
  }
  if (satelliteData.id == 2){
    if (satelliteData.temp > pivot_temp){
      digitalWrite(led_board_two, HIGH);
  } else {
      digitalWrite(led_board_two, LOW);
    }  
  }  
  
  // Serial.printf("Board ID %u: %u bytes\n", satelliteData.id, len);
  // Serial.printf("t value: %4.2f \n", satelliteData.temp);
  // Serial.printf("p value: %4.2f \n", satelliteData.pressure);
  // Serial.printf("a value: %4.2f \n", satelliteData.altitude);
  // Serial.printf("readingID value: %d \n", satelliteData.readingId);
  Serial.println(board["id"]);
  Serial.println(board["id"]);
  Serial.println(board["humidity"]);
  Serial.println(board["temperature"]);
  Serial.println(board["pressure"]);
  Serial.println(board["altitude"]);
  Serial.println();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>EcoHomeHealth</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
        font-family: sans-serif;
        text-align: center;
    }
    #navbar h3{
        color: rgb(2, 2, 2);
        font-size: 1.8em;
    }
    p {
        margin: 2px;
    }
    .node-grid {
        gap: 10px;
        background-color: #2196F3;
        padding: 10px;
        display: grid;
        grid-template-areas:
            'header header header header'
            'temp humidity pressure altitude'
            'footer footer footer footer';
    }
    .node-grid div {
        background-color: rgba(255, 255, 255, 0.8);
        text-align: center;
        padding: 10px 0;
        font-size: 22px;
    }
    .grid-container {
        display: grid;
        column-gap: 50px;
        grid-template-columns: 1fr;
        background-color: #2196F3;
        padding: 5px;
    }
    .grid-item {
        background-color: rgba(255, 255, 255, 0.8);
        border: 1px solid rgba(0, 0, 0, 0.8);
        padding: 10px;
        font-size: 20px;
        text-align: center;
    }
    .item1 { grid-area: header; }
    .item2 { grid-area: temp; }
    .item3 { grid-area: humidity; }
    .item4 { grid-area: pressure; }
    .item5 { grid-area: altitude; }
    .item6 { grid-area: footer; }
  </style>
</head>
<body>
  <div id="navbar">
    <h3>EcoHomeHealth</h3>
  </div>
  <div class="grid-container">
    <div class="node-grid">
        <div class=" grid-item item1"><h4>Location: Living Room (Internal)</h4></div>
        
        <div class="grid-item item2">Temp
            <p><span class="reading"><span id="t1"></span> &deg;C</span></p>
        </div>
        <div class="grid-item item3">Humidity
            <p><span class="reading"><span id="h1"></span> %;</span></p>
        </div>
        <div class="grid-item item4">Pressure
            <p><span class="reading"><span id="p1"></span> Pa;</span></p>
        </div>
        <div class="grid-item item5">Altitude
            <p><span class="reading"><span id="a1"></span> ft;</span></p>
        </div>  
        <div class="grid-item item6">
            <p class="timestamp">Last Reading: <span id="rt1"></span></p>
        </div>
    </div>
    <br><br>
    <div class="node-grid">
        <div class=" grid-item item1"><h4>Location: Directly Outside Living Room (External)</h4></div>
        <div class="grid-item item2">Temp
            <p><span class="reading"><span id="t2"></span> &deg;C</span></p>
        </div>
        <div class="grid-item item3">Humidity
            <p><span class="reading"><span id="h2"></span> %;</span></p>
        </div>
        <div class="grid-item item4">Pressure
            <p><span class="reading"><span id="p2"></span> Pa;</span></p>
        </div>
        <div class="grid-item item5">Altitude
            <p><span class="reading"><span id="a2"></span> ft;</span></p>
        </div>  
        <div class="grid-item item6">
            <p class="timestamp">Last Reading: <span id="rt2"></span></p>
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
 
 source.addEventListener('new_data', function(e) {
  console.log("new_data", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("t"+obj.id).innerHTML = obj.temperature.toFixed(2);
  document.getElementById("h"+obj.id).innerHTML = obj.humidity.toFixed(2);
  document.getElementById("p"+obj.id).innerHTML = obj.pressure.toFixed(2);
  document.getElementById("a"+obj.id).innerHTML = obj.altitude.toFixed(2);
  document.getElementById("rt"+obj.id).innerHTML = getDateTime();
 }, false);
 
}
</script>
</body>
</html>)rawliteral";



void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW error...");
    return;
  }
  
  // if ESPNOW has been initiated, register receive callback
  esp_now_register_recv_cb(OnDataRecv);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

//  server.on("/json", HTTP_GET, [](AsyncWebServerRequest *request) {
//    request->send(200, "application/json", "{\"message\":\"Welcome\"}");
//  });

  server.on("/board-two", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response;
    serializeJsonPretty(board_one, response);
    request->send(200, "application/json", response);
  });

  server.on("/board-one", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response;
    serializeJsonPretty(board_two, response);
    request->send(200, "application/json", response);
  });
  
  server.addHandler(&events);
  server.begin();

  pinMode(led_board_one, OUTPUT);
  pinMode(led_board_two, OUTPUT);
}
 
void loop() {
//  static unsigned long lastEventTime = millis();
//  static const unsigned long EVENT_INTERVAL_MS = 5000;
//  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
//    events.send("ping",NULL,millis());
//    lastEventTime = millis();
//  }
}
