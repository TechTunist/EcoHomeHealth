#include "DHT.h"
#define Type DHT11

int sensePin = 2;

DHT HT(sensePin, Type);

void setup() {

  pinMode(sensePin, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println();

  // initialise sensor activity
  HT.begin();

}

void loop() {

  Serial.println(HT.readTemperature());
  Serial.println(HT.readHumidity()); 

}
