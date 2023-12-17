/**
 * 
*/

#ifndef  __CMESI_CONFIGURADOR_ESTRUTURAS_H_
#define  __CMESI_CONFIGURADOR_ESTRUTURAS_H_

#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"

#define WIFI_PATH "/WIFI_SETTINGS.txt"
#define MESH_PATH "/MESH_SETTINGS.txt"
#define MQTT_PATH "/MQTT_SETTINGS.txt"

typedef struct {
    char ssid[100];
    char password[100];
}_CMESI_WIFI_ST;

typedef struct {
    char clientID[200];
    char token[200];
}_CMESI_MQTT_ST;

#endif