#include <Arduino.h>
#include <WiFi.h>
#include <ThingsBoard.h>    // ThingsBoard SDK
#define TOKEN               "utCKNCjsiu7S6zGTy3Fh"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "demo.thingsboard.io"


void connectToMQTTServer();
void publishValue(const char *topic, float var);
