/**
 * @file cmesi_api_flash_memory.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef  __CMESI_FLASH_MEMORY_H_
#define  __CMESI_FLASH_MEMORY_H_

#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "SPIFFS.h"

#define CMESI_API_FLASH_MEMORY_SSID_PATH    "/SSID.txt"
#define CMESI_API_FLASH_MEMORY_PASSWORD_PATH    "/PASSWORD.txt"

void cmesi_api_flash_memory_init();
bool cmesi_api_read_flash(const char* file_path, void* data, size_t size);
bool cmesi_api_write_flash(const char* file_path, void* data, size_t size);

#endif