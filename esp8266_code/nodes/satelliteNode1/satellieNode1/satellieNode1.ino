#include "DHT.h"
#include "painlessMesh.h"

#define Type DHT11
#define   MESH_PREFIX     "homeIOT"
#define   MESH_PASSWORD   "phyComIOT"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

// initialise temp and humidity global variables
int temp = 0;
int humidity = 0;

void sendMessage() {
  String msg = "Message from satellite node 1";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// define pin used for sensor
int sensePin = 2;

// initialise DHT object
DHT HT(sensePin, Type);



void setup() {

  // set baud rate
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  // set pin to be used as input 
  pinMode(sensePin, INPUT_PULLUP);

  Serial.println(WiFi.macAddress());  

  // initialise sensor activity
  HT.begin();

}

void loop() {

  // it will run the user scheduler as well
  mesh.update();

  // get DHT11 sensor values
  readTempHumidity();
  
}


// function to return temperature & humidity sensor readings
void readTempHumidity() {
  temp = HT.readTemperature();
  humidity = HT.readHumidity();
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
