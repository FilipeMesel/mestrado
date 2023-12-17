/**
 * @file cmesi_antena_app.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef  __CMESI_ANTENA_APP_H_
#define  __CMESI_ANTENA_APP_H_

#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
//Libraries for OLED Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "../Api/cmesi_api_configurador_ap/cmesi_api_configurador_ap.h"
#include "../Api/cmesi_api_configurador_ap/cmesi_api_configurador_estruturas.h"
#include "../Api/cmesi_api_flash_memory/cmesi_api_flash_memory.h"

#define BUTTON_CONFIG   17

//OLED pins
#define OLED_SDA        4
#define OLED_SCL        15 
#define OLED_RST        16
#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64 // OLED display height, in pixels

void cmesi_app_setup();
void cmesi_app_loop();
#endif