#include "sensors.h"
#include <DHT.h>

#define DHT22PIN 17
// #define LDR_DIGITAL 16
#define DHTTYPE DHT22

static DHT dht(DHT22PIN, DHTTYPE);

void sensors_setup()
{
    dht.begin();
}

SensorReadings sensors_read_all()
{
    SensorReadings r;
    // int valorDigital = digitalRead(LDR_DIGITAL);
    // r.luminosidade = valorDigital;
    r.umidade = dht.readHumidity();
    r.temperatura = dht.readTemperature();

    return r;
}
