/****************************************************************************
  * Файл:           function.h
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#pragma once

#include <Arduino.h>
#include "hardware.h"
#include "global.h"

// adc_mega328.cpp
extern uint8_t adc_read_value();
// calculate.cpp
extern void calculate_value(uint8_t channel_No);
extern void processing_values(uint16_t* calc_value1, uint16_t* calc_value2);
// control_leds.cpp
extern void Leds_Off();
extern void set_leds_buf();
extern void Leds_Handler();
extern void Leds_Ready();
// hardware.cpp
extern void Hardware_Init();
