// Compatibilidade: encaminha para o módulo display_epaper
#pragma once
#include "display_epaper.h"

inline void screen_setup() { display_setup(); }
inline void screen_loop() { /* left intentionally empty; use display_epaper functions */ }
