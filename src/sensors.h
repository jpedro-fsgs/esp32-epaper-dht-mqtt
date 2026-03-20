#pragma once

struct SensorReadings {
    float temperatura;
    float umidade;
    int luminosidade;
};

void sensors_setup();
SensorReadings sensors_read_all();
