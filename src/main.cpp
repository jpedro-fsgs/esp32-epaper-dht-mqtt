#include <Arduino.h>
#include "sensors.h"
#include "mqtt_client.h"

#define WHITE_BUTTON 13
#define LED_BUILTIN 2 // Assuming ESP32 internal LED is on GPIO2

// ---- Timers ----
static unsigned long lastMqttSend = 0;
static const unsigned long mqttInterval = 3000; // 3s entre envios MQTT
static bool ledState = false;

// Buffers for MQTT messages
char luminosidadeStr[8];
char umidadeStr[8];
char temperaturaStr[8];

bool ok1, ok2, ok3;

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup iniciando...");

    // Inicializa LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW); // Start with LED off

    // Inicializa módulos
    sensors_setup();
    mqtt_setup();

    // Desenha rótulos estáticos e atualiza valores iniciais
    SensorReadings r = sensors_read_all();
}

void loop()
{
    // Leitura de sensores (não bloqueante)
    SensorReadings r = sensors_read_all();

    // Mantém conexão MQTT viva
    mqtt_loop();

    // Envio MQTT com intervalo (SEM delay)
    unsigned long now = millis();
    if (now - lastMqttSend >= mqttInterval)
    {
        lastMqttSend = now;

        // Convert float to string using dtostrf
        dtostrf(r.luminosidade, 4, 2, luminosidadeStr);
        dtostrf(r.umidade, 4, 2, umidadeStr);
        dtostrf(r.temperatura, 4, 2, temperaturaStr);

        ok1 = mqtt_publish("ESP32/luminosidade", luminosidadeStr);
        ok2 = mqtt_publish("ESP32/umidade", umidadeStr);
        ok3 = mqtt_publish("ESP32/temperatura", temperaturaStr);
        if (!ok1 || !ok2 || !ok3)
        {
            Serial.println("Falha ao publicar algum valor MQTT!");
        }
        else
        {
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
        }
    }

    yield();
}
