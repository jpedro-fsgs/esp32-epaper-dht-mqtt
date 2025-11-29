#pragma once
#include <Arduino.h>

void mqtt_setup();
void mqtt_loop();
bool mqtt_publish(const char* topic, const char* payload);
bool mqtt_is_connected();
// Se houver nova mensagem, copia para "out" e retorna true. Caso contrário, retorna false.
bool mqtt_consume_last_message(String &out);
