#include <Arduino.h>
#include <Ticker.h>
#include "sensors.h"
#include "mqtt_client.h"
#include "ping_buzzer.h"

#define LED_BUILTIN 2 // Assuming ESP32 internal LED is on GPIO2

// ---- Timers ----
static unsigned long lastMqttSend = 0;
static const unsigned long mqttInterval = 3000; // 3s entre envios MQTT

Ticker blinkTimer;

// Buffers for MQTT messages
char luminosidadeStr[8];
char umidadeStr[8];
char temperaturaStr[8];

bool ok1, ok2, ok3;

void turnLedOff()
{
    analogWrite(LED_BUILTIN, 0);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup iniciando...");

    // Inicializa LED
    pinMode(LED_BUILTIN, OUTPUT);
    analogWrite(LED_BUILTIN, 0); // Start with LED off

    // Inicializa módulos
    sensors_setup();
    mqtt_setup();
    setup_buzzer();
}

void loop()
{
    // Leitura de sensores (não bloqueante)

    // Mantém conexão MQTT viva
    mqtt_loop();

    // Envio MQTT com intervalo (SEM delay)
    unsigned long now = millis();
    if (now - lastMqttSend >= mqttInterval)
    {

        ping_server();
        lastMqttSend = now;

        SensorReadings r = sensors_read_all();

        if (isnan(r.temperatura) || isnan(r.umidade))
        {
            Serial.println("Falha ao ler do sensor DHT22!");
            mqtt_publish("ESP32/log", "Falha ao ler do sensor DHT22!");
        }
        else
        {

            // strcpy(luminosidadeStr, r.luminosidade ? "dark" : "light");
            dtostrf(r.umidade, 4, 2, umidadeStr);
            dtostrf(r.temperatura, 4, 2, temperaturaStr);

            // ok1 = mqtt_publish("ESP32/luminosidade", luminosidadeStr);
            ok2 = mqtt_publish("ESP32/umidade", umidadeStr);
            ok3 = mqtt_publish("ESP32/temperatura", temperaturaStr);
            if (!ok2 || !ok3)
            {
                Serial.println("Falha ao publicar algum valor MQTT!");
            }
            else
            {
                analogWrite(LED_BUILTIN, 50);
                blinkTimer.once_ms(50, turnLedOff);
            }
        }
    }

    yield();
}
