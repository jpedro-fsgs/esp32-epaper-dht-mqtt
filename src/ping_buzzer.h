#include <ESP32Ping.h>
#include <Ticker.h>
#define BUZZER 16

Ticker buzzerTimer;

const IPAddress local_server(192, 168, 0, 24);

void setup_buzzer()
{
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW); // Ensure buzzer is off at startup
}

void buzzer()
{
  digitalWrite(BUZZER, HIGH); // Toggle buzzer state
  buzzerTimer.once(0.1, []()
                   { digitalWrite(BUZZER, LOW); }); // Turn off buzzer after 100ms
}

bool ping_server()
{

  bool success = Ping.ping(local_server);
  if (!success)
  {
    Serial.println("Ping falhou! Ativando buzzer...");
    buzzer();
  }

  return success;
}