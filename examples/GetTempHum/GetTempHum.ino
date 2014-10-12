// **********************************************************************************
// Example file reading temperature and humidity from TH02 sensor
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// For any explanation see TH02 sensor information at
// http://www.hoperf.com/sensor/app/TH02.htm

// Code based on following datasheet
// http://www.hoperf.com/upload/sensor/TH02_V1.1.pdf 
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2014-07-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

// Slightly modified for TH02soft library by flabbergast
// 2014-10-12

// Unfortunately, we need to define the I2C pins and parameters in
// the library itself - so please edit libraries/TH02soft/TH02soft.cpp
// The default pins are A4 & A5, just like the usual TWI.

#include <TH02soft.h>
TH02 sensor;

void setup()
{
  uint8_t devID;

  sensor.init();

  Serial.begin(115200);
  Serial.println("\r\nTH02 Demo");
  // TH02 ID is 4 MSB
  devID = sensor.getId() >> 4;

  Serial.print(F("TH02 device ID = 0x"));
  Serial.println(devID,HEX);
  
  if (devID == 0x5)
  {
    Serial.println("TH02 device ID match !");
    Serial.print("TH02 Status = 0x");
    Serial.println(sensor.getStatus());
    Serial.print("TH02 Config = 0x");
    Serial.println(sensor.getConfig());
  }
  
  Serial.println();
}

void loop()
{
  int16_t temp, rh, rh_comp;
  //uint8_t status;

  Serial.print(F("Starting Temperature conversion."));
  sensor.startTempConv();
  sensor.waitEndConversion();
  Serial.println(F(".done!"));
    
  // Get temperature calculated and rounded
  temp = sensor.getConversionValue();

  // Display unrounded value (raw)
  Serial.print(F("Temperature = "));
  // this call does not do any I2C reading, it use last reading
  Serial.print(sensor.getLastRawTemp()/100.0);
  Serial.print(F(" C  =>  "));
  // Display now rounded value returned by 1st call
  Serial.print(temp/10.0);
  Serial.println(F(" C"));
    
    
  Serial.print(F("Starting Humidity conversion."));
  
  // Convert humidity
  sensor.startRHConv();
  sensor.waitEndConversion();
  Serial.println(F(".done!"));

  // Get temperature calculated and rounded with no compensation
  rh = sensor.getConversionValue();
  // Now display last reading with unrounded value 
  // this call does not do any I2C reading, it use last reading
  Serial.print(F("Raw Humidity = "));
  Serial.print(sensor.getLastRawRH()/100.0);
  Serial.print(F("%  => Compensated "));
  Serial.print(sensor.getCompensatedRH(false)/100.0);
  Serial.print(F("%  Rounded  "));
  Serial.print(sensor.getCompensatedRH(true)/10.0);
  Serial.println(F("%"));
  
  Serial.println(F("Now waiting 10 sec before next conversion."));
  delay(10000);
}
