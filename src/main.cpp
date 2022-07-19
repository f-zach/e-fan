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
Adafruit_MAX31856 TypeT1(7);
Adafruit_MAX31856 TypeT2(8);
Adafruit_MAX31856 TypeT3(9);
Adafruit_MAX31856 TypeT4(10);


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

  TypeT1.begin();
  TypeT1.setThermocoupleType(MAX31856_TCTYPE_T);
  TypeT1.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  TypeT2.begin();
  TypeT2.setThermocoupleType(MAX31856_TCTYPE_T);
  TypeT2.setConversionMode(MAX31856_ONESHOT_NOWAIT);

  TypeT3.begin();
  TypeT3.setThermocoupleType(MAX31856_TCTYPE_T);
  TypeT3.setConversionMode(MAX31856_ONESHOT_NOWAIT);
  
  TypeT4.begin();
  TypeT4.setThermocoupleType(MAX31856_TCTYPE_T);  
  TypeT4.setConversionMode(MAX31856_ONESHOT_NOWAIT);

      TypeT1.triggerOneShot();
      TypeT2.triggerOneShot();
      TypeT3.triggerOneShot();
      TypeT4.triggerOneShot();
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

    if(millis()-tStartMeasurement >= 198)
    {
      KTypeTemp[0] = TypeT1.readThermocoupleTemperature();
      KTypeTemp[1] = TypeT2.readThermocoupleTemperature();
      KTypeTemp[2] = TypeT3.readThermocoupleTemperature();
      KTypeTemp[3] = TypeT4.readThermocoupleTemperature();

      TypeT1.triggerOneShot();
      TypeT2.triggerOneShot();
      TypeT3.triggerOneShot();
      TypeT4.triggerOneShot();
      
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