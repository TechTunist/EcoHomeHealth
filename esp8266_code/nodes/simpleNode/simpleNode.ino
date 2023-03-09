// main internal node
// has DHT11, servo, motor and LED to display temperature

// ToDO: 
// add a fan that is powered off a pin that is pulled HIGH if the temp
// variable is above a certain value
// troubleshoot motors here: https://simple-circuit.com/esp8266-nodemcu-dc-motor-control-l293d/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include <Servo.h>

#define Type DHT11

// set the port for the webserver
ESP8266WebServer server(80);

// set wifi details
// HOME
const char* ssid = "SKYXIENC";
const char* password = "7GQiqMQT6zdB";

// OFFICE
//  const char* ssid = "TNCAP24C3C5";
//  const char* password = "7EFEF61DDA";

// set pin for sensing DHT11
int sensePin = 2;

// set pin for turning fan on / off
int fanPin1 = 10;
int fanPin2 = 9;

DHT HT(sensePin, Type);

// initialise temp and humidity global variables
int temp = 0;
int humidity = 0;

// Instantiate JsonDocument object
DynamicJsonDocument doc(1024);

// instantiate servo object
Servo myServo;

void setup() {

  // define thepinmode for fan pins
  pinMode(fanPin1, OUTPUT);

  // connect to the wifi network
  WiFi.begin(ssid, password);

  pinMode(sensePin, INPUT_PULLUP);
  
  Serial.begin(9600);
  Serial.println();

  // wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect...");
  }

  // print the IP address of the web server
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // initialise sensor activity
  HT.begin();

  server.on("/", get_index); // get the index page
  server.on("/setLEDStatus", setLEDStatus);
  
  // get json data
  server.on("/json", get_json);

  // start the server
  server.begin();
  Serial.println("Server Listening");

  // initialise servo
  myServo.attach(0);

}

void loop() {

  // debug fan control
  digitalWrite(fanPin1, HIGH);
  Serial.println("fan turned on");
  delay(2000);
  digitalWrite(fanPin1, LOW);
  Serial.println("fan turned off");
  delay(2000);
  
  

  // get DHT11 sensor values
  readTempHumidity();

///////// debug DHT11 data
//  Serial.print("Temperature: ");
//  Serial.print(HT.readTemperature());
//  Serial.print("C ");
//  Serial.println();
//  Serial.print("Humidity: ");
//  Serial.print(HT.readHumidity());
//  Serial.print(" %");
//  Serial.println();
//
//  delay(5000);

  // handleing of incoming client requests
  server.handleClient();

  // call function to handle servo movement
  servoMovement();
}


//////// utility functions //////////

// function to handle servo movement
void servoMovement() {

  // set the servo to 0 degrees
  myServo.write(0);
  delay(500);

  // set the servo to 45 degrees
  myServo.write(45);
  delay(500);

  // set the servo to 90 degrees
  myServo.write(90);
  delay(500);

  // set the servo to 180 degrees
  myServo.write(180);
  delay(500);
}

// function to get json data and return http response with json data
void get_json() {

  // get json data from dht
  jsonDHTSensor();

  // make json data ready for http request
  String jsonStr;
  serializeJsonPretty(doc, jsonStr);

  // send json data in http response
  server.send(200, "application/json", jsonStr);
}

// function to convert DHT data to json
void jsonDHTSensor(){
  
  // add json request data
  doc["Content-Type"] = "application/json";
  doc["Status"] = 200;

  // add DHT sensor JSON object data
  JsonObject DHTSensor = doc.createNestedObject("Sensor");
  DHTSensor["sensorName"] = "DHT11 Sensor";

  // add DHT sensor data as an array
  JsonArray pins = DHTSensor.createNestedArray("sensorPins");

  // add each pin associated with that sensor
  pins.add(sensePin);

  // set the value for the json sensor object
  DHTSensor["Temperature in Celsius"] = temp;
  DHTSensor["Humidity"] = humidity;
 }  


// function to return temperature & humidity sensor readings
void readTempHumidity() {
  temp = HT.readTemperature();
  humidity = HT.readHumidity();
  
}

// utility function to serve the home page dashboard
void get_index() {
  String html = "<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The EcoHomeHealth Dashboard</h1>";
  html += "<p> Welcome to the Future of Efficiency</p>";
  html += "<div> <p> <strong> The Temperature of sensor 1 is: ";
  // html += HT.readTemperature();
  html += temp;
  html += " C</strong> </p> </div>";
  html += "<div> <p> <strong> The Humidity of sensor 1 is: ";
  // html += HT.readHumidity();
  html += humidity;
  html += " %</strong> </p> </div>";
  //// enter ternery operator here to say if the temperature is above or below threshold;
  // isAboveThreshold()?"is currently above threshold":"is currently below threshold";
  html += "<a href=\"/setLEDStatus?s=0\" target=\"_blank\"\"\"><button>Turn Off </button></a>";
  html += "<a href=\"/setLEDStatus?s=1\" target=\"_blank\"\"\"><button>Turn On </button></a>";
  html += "</body> </html>";

  // print a welcoming message on the index page
  server.send(200, "text.html", html);
}

// utility function to read the query string
// substitute for return json data ??????
void setLEDStatus() {

  int query_string = 0;
  String text;

  // check the query string
  if (server.arg("s") != "") { // parameter found
    // parse the value from the query
    query_string = server.arg("s").toInt();
    // check the value and update the **led pin**
    if (query_string==1) {
      Serial.println("User input = 1");
      text = "User input = 1";
      // delay(1500);
    }
    else {
      Serial.println("User input != 1");
      text = "User input != 1";
      // delay(1500);
    }
  }

  server.send(200, "text/plain", text);
}
