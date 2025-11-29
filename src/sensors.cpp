#include "sensors.h"
#include <DHT.h>

#define DHT22PIN 25
#define LDR_ANALOG 35
#define DHTTYPE DHT22

static DHT dht(DHT22PIN, DHTTYPE);

void sensors_setup()
{
    dht.begin();
}

SensorReadings sensors_read_all()
{
    SensorReadings r;
    int valor = analogRead(LDR_ANALOG);
    r.luminosidade = (4095 - valor) * 100.0 / 4095.0;
    r.umidade = dht.readHumidity();
    r.temperatura = dht.readTemperature();
    return r;
}
