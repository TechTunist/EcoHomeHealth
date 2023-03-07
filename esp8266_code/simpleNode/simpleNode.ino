#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define Type DHT11

// set the port for the webserver
ESP8266WebServer server(80);

// set wifi details
const char* ssid = "SKYXIENC";
const char* password = "7GQiqMQT6zdB";

int sensePin = 2;

DHT HT(sensePin, Type);

void setup() {

  // connect tothe wifi network
  WiFi.begin(ssid, password);

  pinMode(sensePin, INPUT_PULLUP);
  
  Serial.begin(115200);
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

  // start the server
  server.begin();
  Serial.println("Server Listening");

}

void loop() {

  Serial.print("Temperature: ");
  Serial.print(HT.readTemperature());
  Serial.print("C ");
  Serial.println();
  Serial.print("Humidity: ");
  Serial.print(HT.readHumidity());
  Serial.print(" %");
  Serial.println();

  delay(5000);

  // handleing of incoming client requests
  server.handleClient();

}

// utility function to serve the home page dashboard
void get_index() {
  String html = "<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The EcoHomeHealth Dashboard</h1>";
  html += "<p> Welcome to the Future of Efficiency</p>";
  html += "<div> <p> <strong> The Temperature of sensor 1 is: ";
  html += HT.readTemperature();
  html += "</strong> </p>";
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

