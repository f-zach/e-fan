#include <Arduino.h>
#include <i2c_t3.h>
#include <SPI.h>
#include <MAIN.h>
#include <PRS.h>
#include <FRQ.h>
#include <ADC.h>

MAINmodule MAIN(24, 0x76);
PRSmodule PRS(0x70);
ADCmodule ADC(0x48, 0x49);
FRQmodule FRQ(115200);

float SensorRanges[8] = {1.6, 1.6, 1.6, 400, 400, 400, 400, 400};
int UnitSensors[8] = {1, 1, 1, 0, 0, 0, 0, 0};
int UnitReq[8] = {0,0,0,0,0,0,0,0};

char command;
int i;

void setup()
{
  Serial.begin(9600);

  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19);

  SPI.begin();

  MAIN.config();

  PRS.config(B11111111, SensorRanges, B11111000, UnitSensors,UnitReq);

  ADC.config(B00000001);
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
    MAIN.startTmeasurement();
    PRS.readPressAll();

    ADC.readAll();

    FRQ.getFrequency();

    MAIN.readEnvP();
    MAIN.readEnvT();

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

    Serial.print(FRQ.frequency);
    Serial.print("\t");

    Serial.print(MAIN.envPressure);
    Serial.print("\t");

    Serial.print(MAIN.envTemperature);
    Serial.print("\n");

    command = 0;
    break;
  }
}