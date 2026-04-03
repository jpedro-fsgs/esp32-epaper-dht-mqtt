#include "ping_buzzer.h"

#include <Arduino.h>
#include <ESP32Ping.h>
#include <Ticker.h>
#include <WiFi.h>

constexpr uint8_t BUZZER_PIN = 16;
const IPAddress LOCAL_SERVER_IP(192, 168, 0, 24);
Ticker buzzerTimer;

void buzzer_beep()
{
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerTimer.once(0.2, []()
                     { digitalWrite(BUZZER_PIN, LOW); });
}

void setup_buzzer()
{
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

bool ping_server()
{
    const bool success = Ping.ping(LOCAL_SERVER_IP);
    if (!success && WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Ping falhou! Ativando buzzer...");
        buzzer_beep();
    }

    return success;
}
