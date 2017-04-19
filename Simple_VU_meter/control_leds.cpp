/****************************************************************************
  * Файл:           control_leds.cpp
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#include "function.h"

#define LOGIC_1     0xFC  // 1 = ¯¯¯|_ = 0.75 мкс + 0.25 мкс
#define LOGIC_0     0xC0  // 0 = ¯|___ = 0.25 мкс + 0.75 мкс

uint8_t sending_timer = 0;         // передача раз в TIME_UPDATE_LEDS мс
uint8_t leds_buf[ELEMENTS_NUMBER]; // основной буфер для передачи значений на светодиоды

uint8_t base_color[BASE_COLOR_NUMBER] = {COLOR1, COLOR2, COLOR3}; // порядок базовых цветов

// таблица цветовых значений для столбиков
const uint8_t user_multicolored_table[BASE_COLOR_NUMBER][LEDS_CH_NUMBER] PROGMEM = 
{
 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

/******************************************************************************/
/* Сформировать буфер для передачи значений светодиодам                       */
/******************************************************************************/
void set_leds_buf()
{
// вспомогательные буферы для формирования столбиков  
  static uint8_t elements_ch1[ELEMENTS_CH_NUMBER],
                 elements_ch2[ELEMENTS_CH_NUMBER];
                 
// количество светящихся светодиодов без обработки
  uint16_t  calc_value_ch1,
            calc_value_ch2;
// количество светящихся светодиодов после обработки
  static uint8_t led_on_ch1,
                 led_on_ch2;
                 
  uint8_t led_index = 0,     // индекс светодиода
          color_index = 0,   // индекс цвета светодиода
          element_index = 0; // индекс байта для передачи по SPI на светодиоды

/******************************************************************************/
/* Вычисление количества светящихся светодиодов.
   значения calc_value_ch1 и calc_value_ch2                                   */
/******************************************************************************/
  processing_values(&calc_value_ch1, &calc_value_ch2);    
  // количество светящихся светодиодов для каналов 1 и 2
  if(calc_value_ch1 > LEDS_CH_NUMBER) led_on_ch1 = LEDS_CH_NUMBER;
  else                                led_on_ch1 = calc_value_ch1;
  if(calc_value_ch2 > LEDS_CH_NUMBER) led_on_ch2 = LEDS_CH_NUMBER;
  else                                led_on_ch2 = calc_value_ch2;
/******************************************************************************/
/* Формирование разноцветных столбиков                                        */
/******************************************************************************/
  // для канала 1
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    if(led_index<led_on_ch1)
    {
      for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
        elements_ch1[element_index+color_index] = pgm_read_byte(&user_multicolored_table[base_color[color_index]][led_index]);
    }
    else
    {
      for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
        elements_ch1[element_index+color_index] = 0;    
    }
    element_index += BASE_COLOR_NUMBER;
  }
  // для канала 2
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    if(led_index<led_on_ch2)
    {
      for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
        elements_ch2[element_index+color_index] = pgm_read_byte(&user_multicolored_table[base_color[color_index]][led_index]);
    }
    else
    {
      for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
        elements_ch2[element_index+color_index] = 0;    
    }
    element_index += BASE_COLOR_NUMBER;
  }
/******************************************************************************/
/* Выбор очерёдности отображения каналов: 1->2 или 2->1.
   Прямое или обратное отображение светодиодов в каждом из каналов            */
/******************************************************************************/
#if DISPLAY_PRIORITY_CH1 <= DISPLAY_PRIORITY_CH2
// Сначала канал 1
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
 #if  DIRECTION_LEDS_CH1 == 0
      leds_buf[element_index+color_index] = elements_ch1[ELEMENTS_CH_NUMBER-element_index+color_index-BASE_COLOR_NUMBER];
 #else
      leds_buf[element_index+color_index] = elements_ch1[element_index+color_index];
 #endif
    element_index += BASE_COLOR_NUMBER;
  }
// затем канал 2
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
 #if  DIRECTION_LEDS_CH2 == 0
      leds_buf[ELEMENTS_CH_NUMBER+element_index+color_index] = elements_ch2[ELEMENTS_CH_NUMBER-element_index+color_index-BASE_COLOR_NUMBER];
 #else
      leds_buf[ELEMENTS_CH_NUMBER+element_index+color_index] = elements_ch2[element_index+color_index];
 #endif
    element_index += BASE_COLOR_NUMBER;
  }
#else
// Сначала канал 2
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
 #if  DIRECTION_LEDS_CH2 == 0
      leds_buf[element_index+color_index] = elements_ch2[ELEMENTS_CH_NUMBER-element_index+color_index-BASE_COLOR_NUMBER];
 #else
      leds_buf[element_index+color_index] = elements_ch2[element_index+color_index];
 #endif
    element_index += BASE_COLOR_NUMBER;
  }
// затем канал 1
  element_index = 0;
  for(led_index=0; led_index<LEDS_CH_NUMBER; led_index++)
  {
    for(color_index=0; color_index<BASE_COLOR_NUMBER; color_index++)
 #if  DIRECTION_LEDS_CH1 == 0
      leds_buf[ELEMENTS_CH_NUMBER+element_index+color_index] = elements_ch1[ELEMENTS_CH_NUMBER-element_index+color_index-BASE_COLOR_NUMBER];
 #else
      leds_buf[ELEMENTS_CH_NUMBER+element_index+color_index] = elements_ch1[element_index+color_index];
 #endif 
    element_index += BASE_COLOR_NUMBER;
  }
#endif
}
/******************************************************************************/
/* Гашение всех светодиодов при включении                                     */
/******************************************************************************/
void Leds_Off()
{
  for(uint16_t bit_i=0; bit_i<8*ELEMENTS_NUMBER; bit_i++)
  {
    SPDR = LOGIC_0;
    while(!(SPSR & (1<<SPIF)));
  }
}
/******************************************************************************/
/* Передача значений светодиодам без использования прерываний                 */
#if TRANSMISSION_INTERRUPT_DATA == 0
/******************************************************************************/
void send_data()
{
  uint8_t bit_index,
          element_index;
                 
  for(element_index=0; element_index<ELEMENTS_NUMBER; element_index++)
  {
    for(bit_index=0x80; bit_index>0; bit_index>>=1)
    {
      if(leds_buf[element_index] & bit_index)
      {
        while(!(SPSR & (1<<SPIF)));
        SPDR = LOGIC_1;
      }
      else
      {
        while(!(SPSR & (1<<SPIF)));
        SPDR = LOGIC_0;
      }
    }
  }
}
/******************************************************************************/
/* Передача значений светодиодам c использованием прерываний                  */
#else
/******************************************************************************/
/* Старт передачи значений светодиодам                                        */
/******************************************************************************/
void send_data()
{
  SPCR |= (1<<SPIE); // передаём данные в прерывании
  SPDR = 0x00;
}
/******************************************************************************/
/* Непосредственно передача значений светодиодам в прерывании                 */
/******************************************************************************/
ISR(SPI_STC_vect)
{
  static uint8_t bit_i = 0x80,
                 element_index = 0;
                 
  if(leds_buf[element_index] & bit_i) SPDR = LOGIC_1;
  else                                SPDR = LOGIC_0;
  if( (bit_i >>= 1) == 0) // передача всех битов одного цвета
  {
    bit_i = 0x80;
    if(++element_index >= ELEMENTS_NUMBER) // передача значений для всех элементов светодиодов
    {
      SPCR &= ~(1<<SPIE); // запрещаем прерывания
      element_index = 0;  // все данные переданы
    }
  }
}
/******************************************************************************/
#endif
/******************************************************************************/
/* Построение столбиков и отправка данных на светодиоды                       */
/******************************************************************************/
void Leds_Handler()
{
  if(sending_timer == 0)
  {
    set_leds_buf(); // подготовка данных к передаче
    send_data();    // передача данных без(с) прерываний(прерываниями)
    sending_timer = 1;
  }
}
/******************************************************************************/
/* Таймер готовности очередной отправки данных на светодиоды (вызывать в 
   периодических прерываниях по таймеру)                                      */
/******************************************************************************/
void Leds_Ready()
{
  if(sending_timer) sending_timer--;
}

