#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "MAIN.h"
#include "PRS.h"
#include "FRQ.h"
#include "ADC.h"
#include "Adafruit_MAX31856.h"


MAINmodule MAIN(24, 0x76);
PRSmodule PRS(0x70);
ADCmodule ADC(0x48, 0x49);
FRQmodule FRQ(115200);
Adafruit_MAX31856 TypeK1(7);
Adafruit_MAX31856 TypeK2(8);
Adafruit_MAX31856 TypeK3(9);
Adafruit_MAX31856 TypeK4(10);


float SensorRanges[8] = {1.6, 1.6, 1.6, 400, 400, 400, 400, 400};
int UnitSensors[8] = {1, 1, 1, 0, 0, 0, 0, 0};
int UnitReq[8] = {0,0,0,0,0,0,0,0};
byte pSens_i2c_address[8] = {0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28};

char command;
int i;
long tStartMeasurement;
float KTypeTemp[4];

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  SPI.begin();

  MAIN.config();
  MAIN.startTmeasurement();

  ADC.config(B00000001);

  PRS.config(B11111111, SensorRanges, B11111000, UnitSensors, UnitReq, pSens_i2c_address, false);

  TypeK1.begin();
  TypeK1.setThermocoupleType(MAX31856_TCTYPE_K);

  TypeK2.begin();
  TypeK2.setThermocoupleType(MAX31856_TCTYPE_K);

  TypeK3.begin();
  TypeK3.setThermocoupleType(MAX31856_TCTYPE_K);
  
  TypeK4.begin();
  TypeK4.setThermocoupleType(MAX31856_TCTYPE_K);  

      TypeK1.triggerOneShot();
      TypeK2.triggerOneShot();
      TypeK3.triggerOneShot();
      TypeK4.triggerOneShot();
      long tStartMeasurement = millis();  
}

void loop()
{
  if (Serial.available())
  {
    command = Serial.read();
  }

  switch (command)
  {
  case 'r':
    

    PRS.readPressAll();

    ADC.readAll();

    FRQ.getFrequency(0);

    if(millis()-tStartMeasurement >= 130)
    {
      KTypeTemp[0] = TypeK1.readThermocoupleTemperature();
      KTypeTemp[1] = TypeK2.readThermocoupleTemperature();
      KTypeTemp[2] = TypeK3.readThermocoupleTemperature();
      KTypeTemp[3] = TypeK4.readThermocoupleTemperature();

      TypeK1.triggerOneShot();
      TypeK2.triggerOneShot();
      TypeK3.triggerOneShot();
      TypeK4.triggerOneShot();
      
      tStartMeasurement = millis();
    }


    MAIN.readEnvP();
    MAIN.readEnvT();

    MAIN.startTmeasurement();

    for (i = 0; i < PRS.SensorCount; i++)
    {
      Serial.print(PRS.Pressure[i]);
      Serial.print("\t");
    }

    for (i = 0; i < ADC.channelCount; i++)
    {
      Serial.print(ADC.Voltage[i], 5);
      Serial.print("\t");
    }

    Serial.print(FRQ.frequency1);
    Serial.print("\t");

    for (i = 0; i < 4; i++)
    {
      Serial.print(KTypeTemp[i], 3);
      Serial.print("\t");
    }


    Serial.print(MAIN.envPressure);
    Serial.print("\t");

    Serial.print(MAIN.envTemperature);
    Serial.print("\n");

    command = 0;
    break;
  }
}