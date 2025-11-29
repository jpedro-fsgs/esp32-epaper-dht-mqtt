#include <Arduino.h>
#include "screen.h"
// #include <WiFi.h>
// #include <PubSubClient.h>
#include "DHT.h"
// #include "credentials.h"

#define WHITE_BUTTON 13

#define DHT22PIN 25
#define LDR_ANALOG 35

#define DHTTYPE DHT22

DHT dht(DHT22PIN, DHTTYPE);

void setup()
{
    pinMode(WHITE_BUTTON, INPUT_PULLUP);
    dht.begin();
    screen_setup();
}

void loop()
{
    if (digitalRead(WHITE_BUTTON) == LOW)
    {

        int valor = analogRead(LDR_ANALOG);
        float luminosidade = (4095 - valor) * 100.0 / 4095.0;
        float umidade = dht.readHumidity();
        float temperatura = dht.readTemperature();

        // Caixa 1: Temperatura
        display.setPartialWindow(10, 10, 220, 150);
        display.firstPage();
        do
        {
            display.fillRect(10, 10, 220, 150, GxEPD_WHITE);

            display.setCursor(20, 30);
            display.printf("Temp: %.1f C", temperatura);

            display.setCursor(20, 60);
            display.printf("Umid: %.1f %%", umidade);

            display.setCursor(20, 90);
            display.printf("Luz: %.1f %%", luminosidade);

        } while (display.nextPage());

        delay(5000);
    }
}