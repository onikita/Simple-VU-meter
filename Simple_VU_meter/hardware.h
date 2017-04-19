/****************************************************************************
  * Файл:           hardware.h
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#pragma once

/******************************************************************************/
/* Ножки микроконтроллера каналов АЦП: опорный канал + два аудиоканала        */
/******************************************************************************/
//                               (порт,пин)
#define ADC_REF                      C,0 // Arduino A0 - канал для измерения постоянной составляющей
#define ADC_CH1                      C,1 // Arduino A1 - первый канал измерения амплитуды звукового сигнала (например, левый)
#define ADC_CH2                      C,2 // Arduino A2 - второй канал измерения амплитуды звукового сигнала (например, правый)

/******************************************************************************/
/* Ножки микроконтроллера аппаратного SPI для управления светодиодами.
   Используется только сигнал MOSI                                            */
/******************************************************************************/
//                               (порт,пин)
#define SPI_MOSI                     B,3 // Arduino 11 передача данных на светодиоды
#define SPI_CS                       B,2 // Arduino 10 (не используется, но настроить нужно, иначе SPI не работает)

/******************************************************************************/
/* Кол-во каналов                                                             */
/******************************************************************************/
#define CHANNEL_NUMBER               2

/******************************************************************************/
/* Кол-во светодиодов на канал                                                */
/******************************************************************************/
#define LEDS_CH_NUMBER               32

/******************************************************************************/
/* Выбор очерёдности отображения каналов: 1->2 или 2->1.
   Чем меньше значение, тем выше приоритет отображения канала                 */
/******************************************************************************/
#define DISPLAY_PRIORITY_CH1         0
#define DISPLAY_PRIORITY_CH2         1

/******************************************************************************/
/* Прямой или обратный порядок отображения светодиодов в каждом из каналов 
   (0 - обратный, любое значение кроме 0 - прямой)                            */
/******************************************************************************/
#define DIRECTION_LEDS_CH1           1
#define DIRECTION_LEDS_CH2           1

/******************************************************************************/
/* Настройка порядка цветов в таблице. В светодиодах WS2812B по умолчанию GRB.
   GRB == 012 -> RGB == 102. Для изменения порядка цветов в таблицах нужно 
   менять макросы COLOR1, COLOR2, COLOR3                                      */
/******************************************************************************/
#define RED                          1
#define GREEN                        0
#define BLUE                         2
#define COLOR1                       RED
#define COLOR2                       GREEN
#define COLOR3                       BLUE

/******************************************************************************/
/* Передача данных в прерывании по SPI (0 - нет, любое значение кроме 0 - да) */
/******************************************************************************/
#define TRANSMISSION_INTERRUPT_DATA  1

/******************************************************************************/
/* Период обновления светодиодов (мс)                                         */
/******************************************************************************/
#define TIME_UPDATE_LEDS             (float)10.0

/******************************************************************************/
/* Кол-во значений в фильтре измеренных значений.
   Кратное степени 2 для быстрых вычислений                                   */
/******************************************************************************/
#define FILTER_MEAS_NUMBER           32