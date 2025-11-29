#pragma once
#include <Arduino.h> // for String

void display_setup();
void drawStaticLayout();
void updateValues(float temperatura, float umidade, float luminosidade);
void updateMessage(const String &msg);
