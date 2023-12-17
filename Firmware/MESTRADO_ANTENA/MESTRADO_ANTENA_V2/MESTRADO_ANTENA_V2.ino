//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
String url1 = "";
String url2 = "";
boolean recebidoNOS[2] = {false, false};

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  /*String msg = "Hello from node ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );*/
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
  if (fk_sensor == 1)
  {
    recebidoNOS[0] = true;
    url1 = "";
    char *GScriptId = "AKfycbyIHi9mSkfNCHN3Q89x0ePUb6joEguYxIGSrY6GH4e7c6X3UJnQk4EFgYeFdUUhabXx"; //planilha teste
    url1 = String("https://script.google.com") + "/macros/s/" + GScriptId + "/exec?";
    url1 += "tempBME=";
    url1 += tempBME;
    url1 += "&humBME=";
    url1 += humBME;
    url1 += "&pressBME=";
    url1 += pressBME;
    url1 += "&tlsLUX=";
    url1 += tlsLUX;
    url1 += "&tempSHT31=";
    url1 += tempSHT31;
    url1 += "&co2SGP=";
    url1 += co2SGP;
    url1 += "&sgpTVOC=";
    url1 += sgpTVOC;
    url1 += "&resBME=";
    url1 += resBME;
    url1 += "&sgpH2=";
    url1 += sgpH2;
    url1 += "&sgpEthanol=";
    url1 += sgpEthanol;
    url1 += "&macAddress=";
    url1 += fk_sensor;
  } else if (fk_sensor == 2)
  {
    recebidoNOS[1] = true;
    url2 = "";
    char *GScriptId = "AKfycbxI8VM52y0tAjNRg1F4J1gbL14jpUCPvjqfbdQkyAc-jlKT7fQ2DsG-9Fq6uYgW41iQKA"; //planilha teste
    url2 = String("https://script.google.com") + "/macros/s/" + GScriptId + "/exec?";
    url2 += "tempBME=";
    url2 += tempBME;
    url2 += "&humBME=";
    url2 += humBME;
    url2 += "&pressBME=";
    url2 += pressBME;
    url2 += "&tlsLUX=";
    url2 += tlsLUX;
    url2 += "&tempSHT31=";
    url2 += tempSHT31;
    url2 += "&co2SGP=";
    url2 += co2SGP;
    url2 += "&sgpTVOC=";
    url2 += sgpTVOC;
    url2 += "&resBME=";
    url2 += resBME;
    url2 += "&sgpH2=";
    url2 += sgpH2;
    url2 += "&sgpEthanol=";
    url2 += sgpEthanol;
    url2 += "&macAddress=";
    url2 += fk_sensor;
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT ,WIFI_AP_STA);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  if (!(recebidoNOS[0] == true && recebidoNOS[1] == true))
  {
    mesh.update();
  } else {

    recebidoNOS[0] = false;
    recebidoNOS[0] = false;
    Serial.println(url1);
    Serial.println(url2);
    mesh.stop();
    WiFi.begin("CINGUESTS", "acessocin");
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    }
    Serial.println(WiFi.localIP());

    int contador = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      contador++;
      Serial.println("Conectando ao Wi-Fi...");
      if(contador >= 10)
      {
        WiFi.disconnect();
        delay(1000);
        WiFi.begin("CINGUESTS", "acessocin");
        contador = 0;
      }
    }

    Serial.println("Conectado ao Wi-Fi!");
    for (int i = 0; i < 2;)
    {
      HTTPClient http;
      Serial.print("Making a request");
      String url = "";
      if (i == 0)
      {
        Serial.print("Sending: ");
        Serial.println(url1);
        url = url1;
        //http.begin(url1.c_str()); //Specify the URL and certificate
      } else {

        Serial.print("Sending: ");
        Serial.println(url2);
        url = url2;
        //http.begin(url2.c_str()); //Specify the URL and certificate
      }
      http.begin(url.c_str()); //Specify the URL and certificate
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      String payload;
      i++;
      if (httpCode > 0) { //Check for the returning code
        payload = http.getString();

        Serial.println(httpCode);
        Serial.println(payload);
        //     testdrawstyles(payload);
        //if (httpCode == 200 or httpCode == 201) tempPing.Saida(0);
      }
      else {
        Serial.println("Error on HTTP request");
        i--;
      }
      http.end();
    }
    ESP.restart();
  }
}
