/**
 * @file cmesi_antena_app.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-04-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cmesi_antena_app.h"

#include "painlessMesh.h"
#include <HTTPClient.h>
// #include "PubSubClient.h"
#include "string.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

_CMESI_WIFI_ST cmesi_app_wifi_st;
_CMESI_WIFI_ST cmesi_app_mesh_st;
_CMESI_MQTT_ST cmesi_app_mqtt_st;
unsigned long int WiFiTimeout = 0;

// variavel para detectar valor do botão
boolean BUTTON_CONFIG_PRESSED = false;

// PainlessMesh credentials ( name, password and port ): You should change these
#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

// WiFi credentials: should match your access point!
#define STATION_SSID "VIVOFIBRA-56ED"
#define STATION_PASSWORD "72233756ED"

#define LED_WIFI 2

#define HOSTNAME "MQTT_Bridge"
#define TEMPOSCAN 5000

Scheduler userScheduler; // to control your personal task

painlessMesh mesh;
// WiFiClient wifiClient;
boolean recebidoNOS[2] = {false, false};

// char *server = "script.google.com";  // Server URL


                
const int httpsPort = 443;
unsigned long int tempoEnvio = millis(), timeoutScann = millis(), espReset = millis();

// Prototypes
// void reconnect();
void receivedCallback(const uint32_t &from, const String &msg);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void cmesi_app_load_structs();

IPAddress getlocalIP();

void cmesi_app_load_structs()
{
  memset(cmesi_app_wifi_st.ssid, 0, sizeof(cmesi_app_wifi_st.ssid));
  memset(cmesi_app_wifi_st.password, 0, sizeof(cmesi_app_wifi_st.password));
  if (!cmesi_api_read_flash(WIFI_PATH, &cmesi_app_wifi_st, sizeof(cmesi_app_wifi_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_app_wifi_st.ssid, "preencher");
    sprintf(cmesi_app_wifi_st.password, "preencher");
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  String SSID = String(cmesi_app_wifi_st.ssid);
  String PASS = String(cmesi_app_wifi_st.password);
  Serial.print("cmesi_app_wifi_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_app_wifi_st: ");
  Serial.println(PASS);

  memset(cmesi_app_mesh_st.ssid, 0, sizeof(cmesi_app_mesh_st.ssid));
  memset(cmesi_app_mesh_st.password, 0, sizeof(cmesi_app_mesh_st.password));
  if (!cmesi_api_read_flash(MESH_PATH, &cmesi_app_mesh_st, sizeof(cmesi_app_mesh_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_app_mesh_st.ssid, "preencher");
    sprintf(cmesi_app_mesh_st.password, "preencher");
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  SSID = String(cmesi_app_mesh_st.ssid);
  PASS = String(cmesi_app_mesh_st.password);
  Serial.print("cmesi_app_mesh_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_app_mesh_st: ");
  Serial.println(PASS);

  memset(cmesi_app_mqtt_st.clientID, 0, sizeof(cmesi_app_mqtt_st.clientID));
  memset(cmesi_app_mqtt_st.token, 0, sizeof(cmesi_app_mqtt_st.token));
  if (!cmesi_api_read_flash(MQTT_PATH, &cmesi_app_mqtt_st, sizeof(cmesi_app_mqtt_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_app_mqtt_st.clientID, "preencher");
    sprintf(cmesi_app_mqtt_st.token, "preencher");
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  SSID = String(cmesi_app_mqtt_st.clientID);
  PASS = String(cmesi_app_mqtt_st.token);
  Serial.print("cmesi_app_mqtt_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_app_mqtt_st: ");
  Serial.println(PASS);
}

IPAddress myIP(0, 0, 0, 0);

// hivemq pubblic broker address and port
// char mqttBroker[] = "demo.thingsboard.io";
// #define MQTTPORT 1883

// topic's suffix: everyone can publish/subscribe to this public broker,
// you have to change the following 2 defines
// #define PUBPLISHSUFFIX "v1/devices/me/telemetry"
// #define SUBSCRIBESUFFIX "painlessMesh/to/"

// #define PUBPLISHFROMGATEWAYSUFFIX PUBPLISHSUFFIX "gateway"

#define CHECKCONNDELTA 60 // check interval ( seconds ) for mqtt connection

// PubSubClient mqttClient;

bool calc_delay = false;
SimpleList<uint32_t> nodes;
uint32_t nsent = 0;
char buff[512];
uint32_t nexttime = 0;
uint8_t initialized = 0;

// messages received from the mqtt broker
// void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
// {
//   char *cleanPayload = (char *)malloc(length + 1);
//   payload[length] = '\0';
//   memcpy(cleanPayload, payload, length + 1);
//   String msg = String(cleanPayload);
//   free(cleanPayload);

//   Serial.printf("mc t:%s  p:%s\n", topic, payload);
// }

// Needed for painless library
String url = "";
String url1 = "";
// messages received from painless mesh network
void receivedCallback(const uint32_t &from, const String &msg)
{
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, msg);

  double tempBME = doc["tempBME"];
  double humBME = doc["humBME"];
  double pressBME = doc["pressBME"];
  double tlsLUX = doc["tlsLUX"];
  double tempSHT31 = doc["tempSHT31"];
  double co2SGP = doc["co2SGP"];
  double sgpTVOC = doc["sgpTVOC"];
  double resBME = doc["resBME"];
  double sgpH2 = doc["sgpH2"];
  double sgpEthanol = doc["sgpEthanol"];
  int fk_sensor = doc["fk_sensor"];
  if(fk_sensor == 1)
  {
    recebidoNOS[0] = true;
  }else if(fk_sensor == 2)
  {
    recebidoNOS[1] = true;
  }

  if(recebidoNOS[0]== true && recebidoNOS[1]== true)
  {
    

    if(WiFi.status() != WL_CONNECTED)
    {
  // Inicialize a conexão Wi-Fi
      WiFi.begin("VIVOFIBRA-56ED", "72233756ED");

      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao Wi-Fi...");
      }

      Serial.println("Conectado ao Wi-Fi!");
    }
    
    for(int i =0; i < 2; i++)
    {
      HTTPClient http;
      Serial.print("Making a request");
      if(i == 0)
      {
        Serial.print("Sending: ");
        Serial.println(url);
        http.begin(url.c_str()); //Specify the URL and certificate
      }else {
        
        Serial.print("Sending: ");
        Serial.println(url1);
        http.begin(url1.c_str()); //Specify the URL and certificate
      }
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      String payload;
      if (httpCode > 0) { //Check for the returning code
        payload = http.getString();

        Serial.println(httpCode);
        Serial.println(payload);
        espReset = millis();
        //     testdrawstyles(payload);
        //if (httpCode == 200 or httpCode == 201) tempPing.Saida(0);
      }
      else {
        Serial.println("Error on HTTP request");
      }
      http.end();
      url = "";
      url1 = "";
    }
  }

  //if (WiFi.status() == WL_CONNECTED){

    tempoEnvio = millis() - tempoEnvio;
    if(fk_sensor == 1) {
      url = "";
      char *GScriptId = "AKfycbyIHi9mSkfNCHN3Q89x0ePUb6joEguYxIGSrY6GH4e7c6X3UJnQk4EFgYeFdUUhabXx"; //planilha teste
      url = String("https://script.google.com") + "/macros/s/" + GScriptId + "/exec?";
      url+= "tempBME=";
      url+= tempBME;
      url+= "&humBME=";
      url+= humBME;
      url+= "&pressBME=";
      url+= pressBME;
      url+= "&tlsLUX=";
      url+= tlsLUX;
      url+= "&tempSHT31=";
      url+= tempSHT31;
      url+= "&co2SGP=";
      url+= co2SGP;
      url+= "&sgpTVOC=";
      url+= sgpTVOC;
      url+= "&resBME=";
      url+= resBME;
      url+= "&sgpH2=";
      url+= sgpH2;
      url+= "&sgpEthanol=";
      url+= sgpEthanol;
      url+= "&macAddress=";
      url+= fk_sensor;
    }else {
      url1 = "";
      char *GScriptId = "AKfycbxI8VM52y0tAjNRg1F4J1gbL14jpUCPvjqfbdQkyAc-jlKT7fQ2DsG-9Fq6uYgW41iQKA"; //planilha teste
      url1 = String("https://script.google.com") + "/macros/s/" + GScriptId + "/exec?";
      url1+= "tempBME=";
      url1+= tempBME;
      url1+= "&humBME=";
      url1+= humBME;
      url1+= "&pressBME=";
      url1+= pressBME;
      url1+= "&tlsLUX=";
      url1+= tlsLUX;
      url1+= "&tempSHT31=";
      url1+= tempSHT31;
      url1+= "&co2SGP=";
      url1+= co2SGP;
      url1+= "&sgpTVOC=";
      url1+= sgpTVOC;
      url1+= "&resBME=";
      url1+= resBME;
      url1+= "&sgpH2=";
      url1+= sgpH2;
      url1+= "&sgpEthanol=";
      url1+= sgpEthanol;
      url1+= "&macAddress=";
      url1+= fk_sensor;
    }
    tempoEnvio = millis();
    
  //   HTTPClient http;
  //   Serial.print("Making a request");
  //   http.begin(url.c_str()); //Specify the URL and certificate
  //   http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  //   int httpCode = http.GET();
  //   String payload;
  //   if (httpCode > 0) { //Check for the returning code
  //     payload = http.getString();

  //     Serial.println(httpCode);
  //     Serial.println(payload);
  //     espReset = millis();
  //     //     testdrawstyles(payload);
  //     //if (httpCode == 200 or httpCode == 201) tempPing.Saida(0);
  //   }
  //   else {
  //     Serial.println("Error on HTTP request");
  //   }
  //   http.end();
  // }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Connected to WiFi");
  display.display();
  timeoutScann = millis();
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> Start: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> Start: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");

  nodes = mesh.getNodeList();
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end())
  {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;

  sprintf(buff, "Nodes:%d", nodes.size());
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u Offset = %d\n", mesh.getNodeTime(), offset);
}

void onNodeDelayReceived(uint32_t nodeId, int32_t delay)
{
  Serial.printf("Delay from node:%u delay = %d\n", nodeId, delay);
}

// void reconnect()
// {
//   char MAC[9];
//   int i;
//   sprintf(MAC, "%08X", (uint32_t)ESP.getEfuseMac()); // generate unique addresss.

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
//   // Loop until we're reconnected
//   while (!mqttClient.connected())
//   {
//     Serial.println("Attempting MQTT connection...");
//     Serial.print("cmesi_app_mqtt_st.clientID: ");
//     Serial.println(cmesi_app_mqtt_st.clientID);
//     Serial.print("cmesi_app_mqtt_st.token: ");
//     Serial.println(cmesi_app_mqtt_st.token);
//     // Attemp to connect
//     String clientID = String(cmesi_app_mqtt_st.clientID);
//     String token = String(cmesi_app_mqtt_st.token);

//     // if (mqttClient.connect("ca489480-b9ca-11ed-b62c-7d8052ad39cf", "F14101996M", NULL))
//     if (mqttClient.connect(cmesi_app_mqtt_st.clientID, cmesi_app_mqtt_st.token, NULL))
//     {
//       Serial.println("Connected");
//     }
//     else
//     {
//       Serial.print("Failed, rc=");
//       Serial.print(mqttClient.state());
//       Serial.println(" try again in 2 seconds");
//       // Wait 2 seconds before retrying
//       delay(2000);
//       mesh.update();
//       mqttClient.loop();
//     }
//   }
// }

IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}

void cmesi_app_setup()
{

  Serial.begin(115200);
  pinMode (LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, LOW);
  cmesi_api_flash_memory_init();
  cmesi_app_load_structs();

  // reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // pinMode(BUTTON_CONFIG, INPUT);
  // if (digitalRead(BUTTON_CONFIG) == true)
  // {
  //   BUTTON_CONFIG_PRESSED = true;
  //   cmesi_api_configurador_ap_init_server();

  //   IPAddress IP = WiFi.softAPIP();
  //   Serial.print("AP IP address: ");
  //   Serial.println(IP);

  //   display.clearDisplay();
  //   display.setTextColor(WHITE);
  //   display.setTextSize(1);
  //   display.setCursor(0, 10);
  //   display.print(IP);
  //   display.display();
  // }
  // else
  // {

    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // set before init() so that you can see startup messages

    // Channel set to 1. Make sure to use the same channel for your mesh and for you other
    // network (STATION_SSID)
    String meshNetwork = String(cmesi_app_mesh_st.ssid);
    String meshPassword = String(cmesi_app_mesh_st.password);
    // mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA);
    mesh.init("whateverYouLike", "somethingSneaky", 5555, WIFI_AP_STA, 6);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    String wifiNetwork = "VIVOFIBRA-56ED";//"CINGUESTS";//"VIVOFIBRA-56ED";//String(cmesi_app_wifi_st.ssid);
    String wifiPassword = "72233756ED";//"acessocin";//"72233756ED";//String(cmesi_app_wifi_st.password);
    // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    Serial.print("wifiNetwork: ");
    Serial.println(wifiNetwork);
    Serial.print("wifiPassword: ");
    Serial.println(wifiPassword);
    mesh.stationManual(wifiNetwork, wifiPassword);
    mesh.setHostname(HOSTNAME);

    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);

    // if you want your node to accept OTA firmware, simply include this line
    // with whatever role you want your hardware to be. For instance, a
    // mesh network may have a thermometer, rain detector, and bridge. Each of
    // those may require different firmware, so different roles are preferrable.
    //
    // MAKE SURE YOUR UPLOADED OTA FIRMWARE INCLUDES OTA SUPPORT OR YOU WILL LOSE
    // THE ABILITY TO UPLOAD MORE FIRMWARE OVER OTA. YOU ALSO WANT TO MAKE SURE
    // THE ROLES ARE CORRECT
    mesh.initOTAReceive("bridge");

    sprintf(buff, "Id:%d", mesh.getNodeId());
    // tft.drawString(buff, 0, 16);

    // mqttClient.setServer(mqttBroker, MQTTPORT);
    // mqttClient.setCallback(mqttCallback);
    // mqttClient.setClient(wifiClient);

  // }
}
void cmesi_app_loop()

{
  // if (BUTTON_CONFIG_PRESSED == false)
  // {
    // it will run the user scheduler as well
    mesh.update();
    // mqttClient.loop();

    if (myIP != getlocalIP())
    {
      myIP = getlocalIP();
      Serial.println("My IP is " + myIP.toString());
      initialized = 1;
      
      digitalWrite(LED_WIFI, HIGH);
    }
    // else if (WiFi.status() != WL_CONNECTED) {
    //   if(millis() - WiFiTimeout > 10000) {
    //     WiFiTimeout = millis();
    //     WiFi.disconnect();
    //     WiFi.reconnect();
    //   }
    // }

    // if(WiFi.status() == WL_CONNECTED && millis() - tempoEnvio >= 5000)
    // {
    //   HTTPClient http;
    //   for(int i =0; i < 2; i++)
    //   {
    //     Serial.print("Making a request");
    //     if(i == 0)
    //       http.begin(url.c_str()); //Specify the URL and certificate
    //     else
    //       http.begin(url1.c_str()); //Specify the URL and certificate
    //     http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    //     int httpCode = http.GET();
    //     String payload;
    //     if (httpCode > 0) { //Check for the returning code
    //       payload = http.getString();

    //       Serial.println(httpCode);
    //       Serial.println(payload);
    //       espReset = millis();
    //       //     testdrawstyles(payload);
    //       //if (httpCode == 200 or httpCode == 201) tempPing.Saida(0);
    //     }
    //     else {
    //       Serial.println("Error on HTTP request");
    //   }
    //   http.end();
    //   }
    //   tempoEnvio = millis();
    // }if(WiFi.status() != WL_CONNECTED){
    //   Serial.println("Sem rede");
    // }

    
}