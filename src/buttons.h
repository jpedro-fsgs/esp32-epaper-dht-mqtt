#pragma once

// Inicializa o botão com internal pullup
void buttons_setup(int pin, unsigned long debounceMs = 80, unsigned long cooldownMs = 700);

// Retorna true apenas uma vez quando o botão é pressionado (considerando debounce+cooldown)
bool button_was_pressed();
