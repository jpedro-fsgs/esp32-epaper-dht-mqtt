// Deprecated compatibility header — use `mqtt_client.h` instead
#pragma once
#include "mqtt_client.h"

inline void wifi_setup() { mqtt_setup(); }