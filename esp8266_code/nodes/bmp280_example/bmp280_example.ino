///***************************************************************************
//  This is a library for the BMP280 humidity, temperature & pressure sensor
//
//  Designed specifically to work with the Adafruit BMP280 Breakout
//  ----> http://www.adafruit.com/products/2651
//
//  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
//  to interface.
//
//  Adafruit invests time and resources providing this open source code,
//  please support Adafruit andopen-source hardware by purchasing products
//  from Adafruit!
//
//  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
//  BSD license, all text above must be included in any redistribution
// ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

// instantiate BMP280 object
Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

// Instantiate JsonDocument object
DynamicJsonDocument doc(1024);

void setup() {
  Serial.begin(9600);
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(0x76, BMP280_CHIPID);
//  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);

  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {

  // BMP280
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");

  Serial.println();
  delay(2000);
}



/////////////// get sensor address /////////////////
//#include <Wire.h> //include Wire.h library
// 
//void setup()
//{
//  Wire.begin(); // Wire communication begin
//  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
//  while (!Serial); // Waiting for Serial Monitor
//  Serial.println("\nI2C address Scanner CircuitSchools.com");
//}
// 
//void loop()
//{
//  byte error, address; //variable for error and I2C address
//  int devicecount;
// 
//  Serial.println("Scanning...");
// 
//  devicecount = 0;
//  for (address = 1; address < 127; address++ )
//  {
//    // The i2c_scanner uses the return value of
//    // the Write.endTransmisstion to see if
//    // a device did acknowledge to the address.
//    Wire.beginTransmission(address);
//    error = Wire.endTransmission();
// 
//    if (error == 0)
//    {
//      Serial.print("I2C device found at address 0x");
//      if (address < 16)
//        Serial.print("0");
//      Serial.print(address, HEX);
//      Serial.println("  !");
//      devicecount++;
//    }
//    else if (error == 4)
//    {
//      Serial.print("Unknown error at address 0x");
//      if (address < 16)
//        Serial.print("0");
//      Serial.println(address, HEX);
//    }
//  }
//  if (devicecount == 0)
//    Serial.println("No I2C devices found\n");
//  else
//    Serial.println("done\n");
// 
//  delay(5000); // wait 5 seconds for the next I2C scan
//}
