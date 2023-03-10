#include "SimpleEspNowConnection.h"
#include "DHT.h"
#define Type DHT11

// define variable to hold LED state to notify to user which node is which
String state;

// led on pin 3 of ESP01s


SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);

// initialise variable for storing server address sent in callback from server device
String serverAddress;

// define pin used for sensor
int sensePin = 2;

// initialise DHT object
DHT HT(sensePin, Type);

// define data structure to be transmitted
typedef struct dhtStruct {
  char type;
  float temp;
  float humidity;
} dhtStruct;

// define function to send the struct and relevant data to the server
bool sendStruct() {
  dhtStruct dhtData;

  // sensor number 1
  dhtData.type = '1';
  dhtData.temp = HT.readTemperature();
  dhtData.humidity = HT.readHumidity();
  
  return (simpleEspConnection.sendMessage((uint8_t *)&dhtData, sizeof(dhtData)));
}

// callback for error send
void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

// callback for initial handshake
void OnNewGatewayAddress(uint8_t *ga, String ad)
{  
  Serial.println("New GatewayAddress '"+ad+"'");
  serverAddress = ad;

  simpleEspConnection.setServerMac(ga);
}


void setup() {

  // set pin to be used as input 
  pinMode(sensePin, INPUT_PULLUP);

  // set baud rate
  Serial.begin(115200);

  simpleEspConnection.begin();

  simpleEspConnection.setServerMac(serverAddress);
  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onSendError(&OnSendError);  
  
  Serial.println(WiFi.macAddress());  

  // initialise sensor activity
  HT.begin();

  // send automated pair request
  simpleEspConnection.startPairing(30);

}

void loop() {

  simpleEspConnection.loop();

  // send data with 3 second delay for testing purposes
  sendStruct();
  delay(10000);

  Serial.println(HT.readTemperature());

  /*
  When properly deployed, the sensors will sleep and wake up every 5 - 15 minutes.
  This is to save power, especially with a view to deploying battery powered units,
  but also as the temperature difference will be measured over a long period and
  minor fluctuations are not useful data.
  */
  
}
