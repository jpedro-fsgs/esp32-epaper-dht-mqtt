#include "mqtt_client.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

static const char *mqtt_server = "192.168.0.28";

static WiFiClient espClient;
static PubSubClient client(espClient);

static String lastMqttMessage = "";
static bool lastMqttMessageUpdated = false;

static void callback(char *topic, byte *payload, unsigned int length)
{
  String msg = "";
  for (unsigned int i = 0; i < length; i++)
  {
    msg += (char)payload[i];
  }
  lastMqttMessage = msg;
  lastMqttMessageUpdated = true;
}

void mqtt_setup()
{
  Serial.println("Iniciando conexão WiFi...");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  unsigned long start = millis();
  const unsigned long wifiTimeout = 15000; // 15s timeout
  while (WiFi.status() != WL_CONNECTED && millis() - start < wifiTimeout)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Connected to WiFi. IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("Falha ao conectar no WiFi (timeout).");
  }

  Serial.println("Conectando ao servidor MQTT...");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  unsigned long mqttStart = millis();
  const unsigned long mqttTimeout = 10000; // 10s timeout
  int attempts = 0;
  const int maxAttempts = 5;

  while (!client.connected() && millis() - mqttStart < mqttTimeout && attempts < maxAttempts)
  {
    attempts++;
    Serial.print("Tentando MQTT (tentativa ");
    Serial.print(attempts);
    Serial.print(")...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("OK");
      break;
    }
    else
    {
      Serial.println("falha -> tentando novamente");
      delay(1000);
    }
  }

  if (client.connected())
  {
    Serial.println("Conectado ao servidor MQTT");
    client.subscribe("ESP32/message");
  }
  else
  {
    Serial.println("Falha ao conectar ao servidor MQTT");
  }
}

void mqtt_loop()
{
  client.loop();
}

bool mqtt_publish(const char* topic, const char* payload)
{
  return client.publish(topic, payload);
}

bool mqtt_is_connected()
{
  return client.connected();
}

bool mqtt_consume_last_message(String &out)
{
  if (lastMqttMessageUpdated)
  {
    out = lastMqttMessage;
    lastMqttMessageUpdated = false;
    return true;
  }
  return false;
}
