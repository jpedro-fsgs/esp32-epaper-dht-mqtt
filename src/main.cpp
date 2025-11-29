#include <Arduino.h>
#include "display_epaper.h"
#include "sensors.h"
#include "mqtt_client.h"
#include "buttons.h"

#define WHITE_BUTTON 13

// ---- Timers ----
static unsigned long lastMqttSend = 0;
static const unsigned long mqttInterval = 3000; // 3s entre envios MQTT

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup iniciando...");

    // Inicializa módulos
    buttons_setup(WHITE_BUTTON);
    sensors_setup();
    display_setup();
    mqtt_setup();

    // Desenha rótulos estáticos e atualiza valores iniciais
    drawStaticLayout();
    SensorReadings r = sensors_read_all();
    updateValues(r.temperatura, r.umidade, r.luminosidade);
}

void loop()
{
    // Leitura de sensores (não bloqueante)
    SensorReadings r = sensors_read_all();

    // Mantém conexão MQTT viva
    mqtt_loop();

    // Se chegou nova mensagem MQTT, atualiza apenas a área da mensagem
    String msg;
    if (mqtt_consume_last_message(msg))
    {
        updateMessage(msg);
    }

    // Envio MQTT com intervalo (SEM delay)
    unsigned long now = millis();
    if (now - lastMqttSend >= mqttInterval)
    {
        lastMqttSend = now;
        if (!mqtt_is_connected())
        {
            Serial.println("MQTT desconectado!");
        }

        bool ok1 = mqtt_publish("ESP32/luminosidade", String(r.luminosidade).c_str());
        bool ok2 = mqtt_publish("ESP32/umidade", String(r.umidade).c_str());
        bool ok3 = mqtt_publish("ESP32/temperatura", String(r.temperatura).c_str());

        if (!ok1 || !ok2 || !ok3)
        {
            Serial.println("Falha ao publicar algum valor MQTT!");
        }
    }

    // Botão com debounce + cooldown: atualiza tela quando pressionado
    if (button_was_pressed())
    {
        updateValues(r.temperatura, r.umidade, r.luminosidade);
    }

    // Não usar delay() para lógicas principais; curto yield para permitir RTOS gerenciar tarefas
    yield();
}
