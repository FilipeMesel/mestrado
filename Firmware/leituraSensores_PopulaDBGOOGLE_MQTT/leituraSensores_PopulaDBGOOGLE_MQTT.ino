#include "SensorLux.h"
#include "SensorSHT31.h"
#include "SensorBME680.h"
#include "SensorSGP30.h"
#include <WiFiClientSecure.h>
#include "MQTT.h"
#include <WiFi.h>
#include <NTPClient.h>

#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

WiFiClient client;
// Replace with your network credentials
const char* ssid = "ADICIONE SUA REDE WIFI AQUI";
const char* password = "ADICIONE A SENHA DA REDE WIFI AQUI";

WiFiUDP udp;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);/* Cria um objeto "NTP" com as configurações.utilizada no Brasil */


// Insert Firebase project API Key
#define API_KEY "ADICIONE A API KEY DO BANCO DE DADOS AQUI"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "ADICIONE A BASEURL DO BANCO DE DADOS AQUI"

#define USER_EMAIL "ADICIONE O USUÁRIO DO BANCO DE DADOS AQUI"
#define USER_PASSWORD "ADICIONE A SENHA DO BANCO DE DADOS AQUI"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
unsigned long sendDataPrevMillis = 0;

String sensorName = "Node0";
String sensorLocation = "Office";

int qtdDesprezada = 5;
int qtdPopulada = 300;
int idxTransmitido = 0;
int timeToSendData = 0;
long tempoEnvio = 0;

void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  connectToMQTTServer();
}
void reconnectWiFi() {
  WiFi.disconnect();
  WiFi.reconnect();
  connectToMQTTServer();
}

#include "MQTT.h"
// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

void connectToMQTTServer() {
  if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
    Serial.println("Failed to connect");
    return;
  }
}


String uid;
String databasePath;
void firebaseInit() {
  // Variable to save USER UID
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";//"/readings";

}

FirebaseJson json;
void firebaseSend() {
  if (Firebase.ready() ){//&& signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    //sendDataPrevMillis = millis();

    ntp.update();
    String dateTime = ntp.getFormattedTime();
    time_t epochTime = ntp.getEpochTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
    Serial.print("Formatted Time: ");
    Serial.println(dateTime);

    int currentHour = ntp.getHours();
    Serial.print("Hour: ");
    Serial.println(currentHour);

    int currentMinute = ntp.getMinutes();
    Serial.print("Minutes: ");
    Serial.println(currentMinute);

    int currentSecond = ntp.getSeconds();
    Serial.print("Seconds: ");
    Serial.println(currentSecond);

    //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime);

    int monthDay = ptm->tm_mday;
    Serial.print("Month day: ");
    Serial.println(monthDay);

    int currentMonth = ptm->tm_mon + 1;
    Serial.print("Month: ");
    Serial.println(currentMonth);

    int currentYear = ptm->tm_year + 1900;
    Serial.print("Year: ");
    Serial.println(currentYear);

    //Print complete date:
    String subRota = databasePath + "/" + String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + dateTime;
    Serial.print("Current date: ");
    Serial.println(subRota);
    String convertion = GetTempBME();

    json.set("/tempBME", convertion);
    convertion = GetHumBME();
    json.set("/humBME", convertion);
    convertion = GetPressBME();
    json.set("/pressBME", convertion);
    convertion = getLuxValue();
    json.set("/tlsLUX", convertion);
    convertion = getTemperatureSHT31();
    json.set("/tempSHT31", convertion);
    convertion = sgp30GetCO2();
    json.set("/co2SGP", convertion);
    convertion = sgp30GetTVOC();
    json.set("/sgpTVOC", convertion);
    convertion = GetTempBME();
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, subRota.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}

void publishValue(const char *topic, float var) {
  tb.sendTelemetryFloat(topic, var);
}

void setup() {
  Serial.begin(115200);

  initWiFi();
  firebaseInit();
  /* Espera a conexão. */
  ntp.begin();               /* Inicia o protocolo */
  ntp.forceUpdate();    /* Atualização */


  initBME680();
  SensorLuxInit();

  /* Display some basic information on this sensor */
  //displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();

  initSHT31();

  sgp30Init();
}

void loop() {
  BME680IaqSystem();
  sgp30Readings();
  if (WiFi.status() == WL_CONNECTED) {
    firebaseSend();
    if (millis() - timeToSendData > 1000) {
      timeToSendData = millis();

      String convertion = GetTempBME();
      float sensorData = convertion.toFloat();
      publishValue("tempBME", sensorData);
      convertion = GetHumBME();
      sensorData = convertion.toFloat();
      publishValue("humBME", sensorData);
      convertion = GetPressBME();
      sensorData = convertion.toFloat();
      publishValue("pressBME", sensorData);
      convertion = getLuxValue();
      sensorData = convertion.toFloat();
      publishValue("tlsLUX", sensorData);
      sensorData = getTemperatureSHT31();
      publishValue("tempSHT31", sensorData);
      convertion = sgp30GetCO2();
      sensorData = convertion.toFloat();
      publishValue("co2SGP", sensorData);
      convertion = sgp30GetTVOC();
      sensorData = convertion.toFloat();
      publishValue("sgpTVOC", sensorData);
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    reconnectWiFi();
  }
  idxTransmitido++;

  // Process messages
  tb.loop();
}
