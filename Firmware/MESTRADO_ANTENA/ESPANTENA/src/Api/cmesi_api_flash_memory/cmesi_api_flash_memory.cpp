/**
 * @file cmesi_api_flash_memory.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-03-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cmesi_api_flash_memory.h"

void cmesi_api_flash_memory_init()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}


bool cmesi_api_read_flash(const char* file_path, void* data, size_t size) {
  // Abre o arquivo em modo leitura
  File file = SPIFFS.open(file_path, FILE_READ);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo");
    return false;
  }

  // Lê os dados da struct da memória flash
  file.read((uint8_t*)data, size);
  file.close();

  return true;
}

bool cmesi_api_write_flash(const char* file_path, void* data, size_t size) {
  // Abre o arquivo em modo escrita
  File file = SPIFFS.open(file_path, FILE_WRITE);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo");
    return false;
  }

  // Escreve os dados da struct na memória flash
  file.write((uint8_t*)data, size);
  file.close();

  return true;
}