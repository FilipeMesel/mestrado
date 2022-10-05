#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

uint32_t getLuxValue();
void displaySensorDetails(void);
void configureSensor(void);
void SensorLuxInit(void);
