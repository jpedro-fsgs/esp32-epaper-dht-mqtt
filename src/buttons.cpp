#include "buttons.h"
#include <Arduino.h>

static int buttonPin = -1;
static unsigned long lastButtonPress = 0;
static unsigned long debounce = 80;
static unsigned long cooldown = 700;

void buttons_setup(int pin, unsigned long debounceMs, unsigned long cooldownMs)
{
    buttonPin = pin;
    debounce = debounceMs;
    cooldown = cooldownMs;
    pinMode(buttonPin, INPUT_PULLUP);
}

bool button_was_pressed()
{
    if (buttonPin < 0)
        return false;

    unsigned long now = millis();
    if (digitalRead(buttonPin) == LOW)
    {
        if (now - lastButtonPress > debounce && now - lastButtonPress > cooldown)
        {
            lastButtonPress = now;
            return true;
        }
    }
    return false;
}
