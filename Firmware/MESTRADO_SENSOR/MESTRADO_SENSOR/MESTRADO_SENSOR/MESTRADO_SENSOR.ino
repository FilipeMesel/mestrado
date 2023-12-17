#include "painlessMesh.h"
#include <ArduinoJson.h> // Certifique-se de incluir a biblioteca
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include "Adafruit_BME680.h"
#include "Adafruit_SHT31.h"
#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(0x49, 12345);

Adafruit_BME680 bme; // I2C

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();


#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
double bmeTemperature = 0, bmeResistance = 0, bmeHumidity = 0, bmePress = 0, lux = 0, shtTemp = 0;
double sgpCO2 = 0, sgpETHANOL = 0, SGPH2 = 0, sgpTVOC = 0;
SGP30 mySensor;
void sgp30Init();
void sgp30Readings();

int ledGpio = 2;
void sgp30Init(){
  pinMode(ledGpio,OUTPUT);
  digitalWrite(ledGpio,1);
  Wire.begin();
  uint16_t cmd = 0x2003;
  Wire.beginTransmission(0x58);
  Wire.write(cmd >> 8);
  Wire.write(cmd & 0xFF);
  Wire.endTransmission();
  Wire.begin();
  //Initialize sensor
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    int resetBuf[1];
    int idxReset = 0;
    while (1){
        resetBuf[idxReset] = idxReset;
        idxReset++;
        ESP.restart();
    }
  }
  digitalWrite(ledGpio,0);
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  mySensor.initAirQuality();
  Serial.println("waiting first 20 sensor readings....it's recomended by library");
  for(int i=0;i<20;i++){
    delay(1000); //Wait 1 second
    //measure CO2 and TVOC levels
    mySensor.measureAirQuality();
    Serial.print("desprezou ");
    Serial.print(i);
    Serial.println(" leituras");
  }    
}

void sgp30Readings(){
    mySensor.measureAirQuality();
    Serial.print("CO2: ");
    sgpCO2 = mySensor.CO2;
    sgpTVOC = mySensor.TVOC;
    Serial.print(sgpCO2);
    Serial.print(" ppm\tTVOC: ");
    Serial.print(sgpTVOC);
    Serial.println(" ppb");
    mySensor.measureRawSignals();
    Serial.print("Raw H2: ");
    sgpETHANOL = mySensor.ethanol;
    SGPH2 = mySensor.H2;
    Serial.print(mySensor.H2);
    Serial.print(" \tRaw Ethanol: ");
    Serial.println(mySensor.ethanol);
}


void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

//ler lux
void reedLux()
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    lux = event.light;
    Serial.print(lux); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  //delay(250);
}

// ler sht31
void readSHT31()
{
  shtTemp = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(shtTemp)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(shtTemp); Serial.print("\t\t");
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
  }
}

//ler bme
void reedBME680()
{
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
  //delay(50); // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());
  bmeTemperature = bme.temperature;
  bmeResistance = bme.gas_resistance / 1000.0; 
  bmeHumidity = bme.humidity; 
  bmePress = bme.pressure / 100.0;
  

  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  Serial.println();
}

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  reedBME680();
  sgp30Readings();
  readSHT31();
  reedLux();
  StaticJsonDocument<256> jsonDoc;
  // Preencha o JsonDocument com os valores desejados
  jsonDoc["tempBME"] = bmeTemperature;
  jsonDoc["humBME"] = bmeHumidity;
  jsonDoc["pressBME"] = bmePress;
  jsonDoc["tlsLUX"] = lux;
  jsonDoc["tempSHT31"] = shtTemp;
  jsonDoc["co2SGP"] = sgpCO2;
  jsonDoc["sgpTVOC"] = sgpTVOC;
  jsonDoc["resBME"] = bmeResistance;
  jsonDoc["sgpH2"] = SGPH2;
  jsonDoc["sgpEthanol"] = sgpETHANOL;
  jsonDoc["mac"] = String(WiFi.macAddress());

  // Crie uma string para armazenar o JSON convertido
  String jsonString;
  
  // Converta o JsonDocument em uma string JSON
  serializeJson(jsonDoc, jsonString);

  // Envie a mensagem através do mesh
  mesh.sendBroadcast(jsonString);

  // Defina o intervalo da tarefa para um valor aleatório entre 1 e 5 segundos
  //taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
  taskSendMessage.setInterval(TASK_SECOND * 1);
  bmeTemperature = 0; 
  bmeResistance = 0; 
  bmeHumidity = 0; 
  bmePress = 0;
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

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME680 async test"));
  sgp30Init();

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  configureSensor();

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
