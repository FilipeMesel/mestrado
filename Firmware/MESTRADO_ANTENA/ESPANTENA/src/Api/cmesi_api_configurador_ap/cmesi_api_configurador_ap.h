/**
 * @file cmesi_api_configurador_ap.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef  __CMESI_CONFIGURADOR_WIFI_H_
#define  __CMESI_CONFIGURADOR_WIFI_H_

#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "cmesi_api_configurador_estruturas.h"

void cmesi_api_configurador_ap_load_structs();
void cmesi_api_configurador_ap_init_server();
void cmesi_api_configurador_ap_load_routes();
#endif