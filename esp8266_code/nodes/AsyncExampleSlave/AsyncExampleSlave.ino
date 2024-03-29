#include <espnow.h>
#include <ESP8266WiFi.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

// Set the ID so the receiver web server distinguishes between different senders 
#define BOARD_ID 2

// define DHT object type
#define Type DHT11

// set pin for sensing DHT11
int sensePin = 2;

// instantiate DHT11 object
DHT HT(sensePin, Type);

Adafruit_BMP280 bmp; 

//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0x40, 0x91, 0x51, 0x4F, 0x0A, 0x56};

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int id;
  float temp;
  float pressure;
  float altitude;
  float humidity;
  int readingId;
} struct_message;

//Create a struct_message called myData
struct_message myData;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

unsigned int readingId = 0;

// Insert your SSID

// Office
 constexpr char WIFI_SSID[] = "TNCAP24C3C5";

// Home
//constexpr char WIFI_SSID[] = "SKYXIENC";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

void initbmp(){
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid bmp280 sensor, check wiring!");
    while (1);
  }
}

float readTemperature() {
  float t = bmp.readTemperature();
  return t;
}

float readPressure() {
  float p = bmp.readPressure();
  return p;
}

float readAltitude() {
  float a = bmp.readAltitude();
  return a;
}

float readHumidity() {
  float h = HT.readHumidity();
  return h;
  }

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  //Init Serial Monitor
  Serial.begin(115200);
  initbmp();

  // set pinmode for DHT11
  pinMode(sensePin, INPUT_PULLUP);

  // begin sensing DHT11
  HT.begin();

  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    //Set values to send
    myData.id = BOARD_ID;
    myData.temp = readTemperature();
    myData.pressure = readPressure();
    myData.altitude = readAltitude();
    myData.humidity = readHumidity();
    myData.readingId = readingId++;
     
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    Serial.print("loop");
  }
}
