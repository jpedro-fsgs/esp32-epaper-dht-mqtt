#pragma once
#include <Arduino.h>

void mqtt_setup();
void mqtt_loop();
void mqtt_reconnect();
bool mqtt_publish(const char* topic, const char* payload);
bool mqtt_is_connected();
