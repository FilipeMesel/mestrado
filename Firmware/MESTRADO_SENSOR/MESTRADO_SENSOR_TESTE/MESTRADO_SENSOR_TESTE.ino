#include "painlessMesh.h"
#include <ArduinoJson.h> // Certifique-se de incluir a biblioteca

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  StaticJsonDocument<256> jsonDoc;

  // Preencha o JsonDocument com os valores desejados
  jsonDoc["tempBME"] = 10.0;
  jsonDoc["humBME"] = 20.0;
  jsonDoc["pressBME"] = 30.0;
  jsonDoc["tlsLUX"] = 40.0;
  jsonDoc["tempSHT31"] = 50.0;
  jsonDoc["co2SGP"] = 60.0;
  jsonDoc["sgpTVOC"] = 70.0;
  jsonDoc["resBME"] = 80.0;
  jsonDoc["sgpH2"] = 90.0;
  jsonDoc["sgpEthanol"] = 100.0;
  jsonDoc["mac"] = "ABCDEFGHI";

  // Crie uma string para armazenar o JSON convertido
  String jsonString;
  
  // Converta o JsonDocument em uma string JSON
  serializeJson(jsonDoc, jsonString);

  // Envie a mensagem através do mesh
  mesh.sendBroadcast(jsonString);

  // Defina o intervalo da tarefa para um valor aleatório entre 1 e 5 segundos
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
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
