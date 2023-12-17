/**
 * @file cmesi_api_configurador_ap.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cmesi_api_configurador_ap.h"
#include "cmesi_api_configurador_estruturas.h"
#include "../cmesi_api_flash_memory/cmesi_api_flash_memory.h"

AsyncWebServer server(80);

_CMESI_WIFI_ST cmesi_wifi_st;
_CMESI_WIFI_ST cmesi_mesh_st;
_CMESI_MQTT_ST cmesi_mqtt_st;

String processorIndex(const String &var);
String processorRede(const String &var);
String processorMESH(const String &var);
String processorInfo(const String &var);

String processorIndex(const String &var)
{
  return "teste";
}
String processorInfo(const String &var)
{
  return "teste";
}

// Replaces placeholder with LED state value
String processorMESH(const String &var)
{
  String retorno = "";
  if (var == "SSID")
  {
    retorno = String(cmesi_mesh_st.ssid);
    return retorno;
  }
  else if (var == "PASSWORD")
  {
    retorno = String(cmesi_mesh_st.password);
    return retorno;
  }
  return String();
}

// Replaces placeholder with LED state value
String processorRede(const String &var)
{
  // if(var == "PORTA") {
  //   return "1883";
  // }
  // else
  String retorno = "";
  if (var == "SSID")
  {
    retorno = String(cmesi_wifi_st.ssid);
    return retorno;
  }
  else if (var == "PASSWORD")
  {
    retorno = String(cmesi_wifi_st.password);
    return retorno;
  }
  else if (var == "CLIENTID")
  {
    retorno = String(cmesi_mqtt_st.clientID);
    return retorno;
  }
  else if (var == "TOKEN")
  {
    retorno = String(cmesi_mqtt_st.token);
    return retorno;
  }
  // else if(var == "SUBTOPIC") {
  //   return "TOPICO DE SUBSCRIÇÃO";
  // }
  // else if(var == "PUBTOPIC") {
  //   return "TOPICO DE PUBLICAÇÃO";
  // }
  // else if(var == "CLIENTID") {
  //   return "CLIENT ID";
  // }
  // else if(var == "TOKENMQTT") {
  //   return "TOKEN MQTT";
  // }
  // else if(var == "SERVER") {
  //   return "SERVIDOR";
  // }
  return String();
}

void serverResetjxCallback(AsyncWebServerRequest *request)
{
  request->send(200);
  delay(5000);
  ESP.restart();
}

void serverSaveMESHAjxCallback(AsyncWebServerRequest *request)
{
  Serial.println("serverSaveWiFiAjxCallback");
  // String ssid, password, inputSubscribeTopic, inputPublishTopic, tkMqtt, servidor, porta;
  String ssid, password;
  if (request->hasParam("ssid", true))
  {
    ssid = request->getParam("ssid", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'ssid' não encontrado.");
    return;
  }
  if (request->hasParam("password", true))
  {
    password = request->getParam("password", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'password' não encontrado.");
    return;
  }
  Serial.println("SSID RECEBIDO: " + ssid);
  Serial.println("Senha recebida: " + password);
  memset(cmesi_mesh_st.ssid, 0, sizeof(cmesi_mesh_st.ssid));
  memset(cmesi_mesh_st.password, 0, sizeof(cmesi_mesh_st.password));
  strcpy(cmesi_mesh_st.ssid, ssid.c_str());
  strcpy(cmesi_mesh_st.password, password.c_str());
  Serial.print("CONEXOES WIFI: ");
  for (int i = 0; i < sizeof(cmesi_mesh_st.ssid); i++)
  {
    Serial.print(cmesi_mesh_st.ssid[i]);
  }
  Serial.print(" ");
  for (int i = 0; i < sizeof(cmesi_mesh_st.password); i++)
  {
    Serial.print(cmesi_mesh_st.password[i]);
  }
  Serial.println();
  if (!cmesi_api_write_flash(MESH_PATH, &cmesi_mesh_st, sizeof(cmesi_mesh_st)))
  {
    Serial.println("Erro ao escrever a struct na memória flash");
    return;
  }
  request->send(200);
}

void serverSaveWiFiAjxCallback(AsyncWebServerRequest *request)
{
  Serial.println("serverSaveWiFiAjxCallback");
  // String ssid, password, inputSubscribeTopic, inputPublishTopic, tkMqtt, servidor, porta;
  String ssid, password, clientID, token;
  if (request->hasParam("ssid", true))
  {
    ssid = request->getParam("ssid", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'ssid' não encontrado.");
    return;
  }
  if (request->hasParam("password", true))
  {
    password = request->getParam("password", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'password' não encontrado.");
    return;
  }
  if (request->hasParam("clientid", true))
  {
    clientID = request->getParam("clientid", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'clientid' não encontrado.");
    return;
  }
  if (request->hasParam("token", true))
  {
    token = request->getParam("token", true)->value();
  }
  else
  {
    request->send(400, "text/plain", "Erro: parâmetro 'token' não encontrado.");
    return;
  }
  Serial.println("SSID RECEBIDO: " + ssid);
  Serial.println("Senha recebida: " + password);
  memset(cmesi_wifi_st.ssid, 0, sizeof(cmesi_wifi_st.ssid));
  memset(cmesi_wifi_st.password, 0, sizeof(cmesi_wifi_st.password));
  strcpy(cmesi_wifi_st.ssid, ssid.c_str());
  strcpy(cmesi_wifi_st.password, password.c_str());

  memset(cmesi_mqtt_st.clientID, 0, sizeof(cmesi_mqtt_st.clientID));
  memset(cmesi_mqtt_st.token, 0, sizeof(cmesi_mqtt_st.token));
  strcpy(cmesi_mqtt_st.clientID, clientID.c_str());
  strcpy(cmesi_mqtt_st.token, token.c_str());
  Serial.print("CONEXOES WIFI: ");
  for (int i = 0; i < sizeof(cmesi_wifi_st.ssid); i++)
  {
    Serial.print(cmesi_wifi_st.ssid[i]);
  }
  Serial.print(" ");
  for (int i = 0; i < sizeof(cmesi_wifi_st.password); i++)
  {
    Serial.print(cmesi_wifi_st.password[i]);
  }
  Serial.println();
  if (!cmesi_api_write_flash(WIFI_PATH, &cmesi_wifi_st, sizeof(cmesi_wifi_st)))
  {
    Serial.println("Erro ao escrever a struct na memória flash");
    return;
  }
  if (!cmesi_api_write_flash(MQTT_PATH, &cmesi_mqtt_st, sizeof(cmesi_mqtt_st)))
  {
    Serial.println("Erro ao escrever a struct na memória flash");
    return;
  }
  request->send(200);
}

void cmesi_api_configurador_ap_load_routes()
{
  // crie o objeto AsyncWebServer e registre as rotas HTTP necessárias
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // request->send(200, "text/html", "/index.html");
              request->send(SPIFFS, "/index.html", String(), false);
              // request->send(SPIFFS, "/img.jpg", "image/jpg");
            });
  server.on("/img", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/img.png", "image/jpeg"); });

  // crie o objeto AsyncWebServer e registre as rotas HTTP necessárias
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //request->send(200, "text/html", "/index.html");
    request->send(SPIFFS, "/index.html", String(), false, processorIndex); });

  // crie o objeto AsyncWebServer e registre as rotas HTTP necessárias
  server.on("/rede.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //request->send(200, "text/html", "/index.html");
    request->send(SPIFFS, "/rede.html", String(), false, processorRede); });

  // crie o objeto AsyncWebServer e registre as rotas HTTP necessárias
  server.on("/redeSensores.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //request->send(200, "text/html", "/index.html");
    request->send(SPIFFS, "/redeSensores.html", String(), false, processorMESH); });

  // crie o objeto AsyncWebServer e registre as rotas HTTP necessárias
  server.on("/info.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //request->send(200, "text/html", "/index.html");
    request->send(SPIFFS, "/info.html", String(), false, processorInfo); });

  server.on("/saveWiFiAjx", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // chama a função de callback para retornar as informações de ssid e senha
    serverSaveWiFiAjxCallback(request); });

  server.on("/saveMESHAjx", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // chama a função de callback para retornar as informações de ssid e senha
    serverSaveMESHAjxCallback(request); });

  server.on("/resetAjx", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // chama a função de callback para retornar as informações de ssid e senha
    serverResetjxCallback(request); });
      Serial.println("Rotas carregadas");
}


void cmesi_api_configurador_ap_load_structs()
{
  memset(cmesi_wifi_st.ssid, 0, sizeof(cmesi_wifi_st.ssid));
  memset(cmesi_wifi_st.password, 0, sizeof(cmesi_wifi_st.password));
  if (!cmesi_api_read_flash(WIFI_PATH, &cmesi_wifi_st, sizeof(cmesi_wifi_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_wifi_st.ssid, "preencher");
    sprintf(cmesi_wifi_st.password, "preencher");
    // return;
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  String SSID = String(cmesi_wifi_st.ssid);
  String PASS = String(cmesi_wifi_st.password);
  Serial.print("cmesi_wifi_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_wifi_st: ");
  Serial.println(PASS);

  
  memset(cmesi_mesh_st.ssid, 0, sizeof(cmesi_mesh_st.ssid));
  memset(cmesi_mesh_st.password, 0, sizeof(cmesi_mesh_st.password));
  if (!cmesi_api_read_flash(MESH_PATH, &cmesi_mesh_st, sizeof(cmesi_mesh_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_mesh_st.ssid, "preencher");
    sprintf(cmesi_mesh_st.password, "preencher");
    // return;
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  SSID = String(cmesi_mesh_st.ssid);
  PASS = String(cmesi_mesh_st.password);
  Serial.print("cmesi_mesh_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_mesh_st: ");
  Serial.println(PASS);

  
  memset(cmesi_mqtt_st.clientID, 0, sizeof(cmesi_mqtt_st.clientID));
  memset(cmesi_mqtt_st.token, 0, sizeof(cmesi_mqtt_st.token));
  if (!cmesi_api_read_flash(MQTT_PATH, &cmesi_mqtt_st, sizeof(cmesi_mqtt_st)))
  {
    Serial.println("Erro ao ler a struct de wifi da memória flash");
    sprintf(cmesi_mqtt_st.clientID, "preencher");
    sprintf(cmesi_mqtt_st.token, "preencher");
    // return;
    // return;
  }
  else
  {
    Serial.println("Leitura da struct wifi realizada com sucesso!");
  }

  SSID = String(cmesi_mqtt_st.clientID);
  PASS = String(cmesi_mqtt_st.token);
  Serial.print("cmesi_mqtt_st: ");
  Serial.println(SSID);
  Serial.print("cmesi_mqtt_st: ");
  Serial.println(PASS);
}

void cmesi_api_configurador_ap_init_server()
{
  String ssidAP = "DEMETER_" + String(WiFi.macAddress());
  WiFi.softAP(ssidAP.c_str(), "123456789");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  cmesi_api_configurador_ap_load_routes();
  cmesi_api_configurador_ap_load_structs();
  // Start server
  server.begin();
  Serial.println("Servidor startado");
}